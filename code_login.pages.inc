<?php

/**
 * @file
 * One input login page for the Code Login module.
 */


function code_custom_login_form($form, &$form_state) {

  $title = variable_get('custom_login_page_title');
  drupal_set_title($title);
  $form = array();
  $form['custom_login'] = array(
    '#type' => 'textfield',
    '#title' => "Enter Code",
    '#size' => 25,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Login'),
    '#submit' => array('code_custom_login_form_submit'),
  ); 

  return $form;

}

function code_custom_login_form_validate($form, &$form_state) {

  $values = $form_state['values'];
  $code = check_plain($values['custom_login']);

  $codes = db_select('login_codes', 'c')
    ->fields('c', array('code'))
    ->execute()
    ->fetchCol();

  //Check if is a valid code
  if (!in_array($code, $codes)) {
    $msg = variable_get('code_login_error_not_valid', 'This is not a valid code. Please enter a valid code');
    form_set_error('custom_login', $msg);
  }

  //Check if the code has expired
  if (in_array($code, $codes)) {
    $data = db_select('login_codes', 'c')
    ->fields('c', array('expiration', 'uid'))
    ->condition('code', $code, '=')
    ->execute()
    ->fetchAll();

    $uid = $data[0]->uid;

    $expiration = $data[0]->expiration;
    $now = time();

    // If the code has expired block user
    if ($expiration > 0 && $now > $expiration) {
      $account = user_load($uid);
      if ($account->status == 1) {
        $account->status = 0;
        user_save($account);
      }
      $msg = variable_get('code_login_error_expired', 'This code has expired. Please enter a valid code or contact the administration to get a new code');
      form_set_error('custom_login',$msg );
    }
  }

}

function code_custom_login_form_submit($form, &$form_state) {

  $values = $form_state['values']; 

  $code = check_plain($values['custom_login']);

  $fullname = db_select('login_codes', 'lc')
  ->fields('lc', array('full_name'))
  ->condition('code', $code, '=')
  ->execute()
  ->fetchCol();

  $_SESSION['login_code']['fullname'] = !empty($fullname) ? $fullname[0] : '';

  $uid = db_select('login_codes', 'c')
    ->fields('c', array('uid'))
    ->condition('code', $code, '=')
    ->execute()
    ->fetchCol();
  $uid = $uid[0];

  $account = array('uid' => $uid);
  user_login_submit(array(), $account); 

  $redirect = variable_get('code_login_redirect_page') ? variable_get('code_login_redirect_page') : '<front>';
  drupal_goto($redirect);

}
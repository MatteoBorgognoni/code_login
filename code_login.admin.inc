<?php

/**
 * @file
 * Administrative Form for the Code Login module.
 */

function code_login_form($form, &$form_state) {

  global $user;

  $roles = user_roles($membersonly = TRUE, $permission = NULL);
  $types = variable_get('code_login_defined_types');

  $default = !empty($form_state['values']['howmany_codes']) ? $form_state['values']['howmany_codes'] : 0;
  $mail_server = !empty($form_state['values']['code_login_mail_server']) ? $form_state['values']['code_login_mail_server'] : 'MAIL_SERVER_TO_CONFIGURE';

  $form = array();

  $form['generated_codes'] = array(
    '#type' => 'fieldset',
    '#title' => "Generated Codes",
    '#collapsible' => TRUE, 
    '#collapsed' => FALSE,
  );

  $codes_table = generate_codes_table();

  $form['generated_codes']['codes_table'] = array(
      '#markup' => $codes_table,
  );

  $form['generated_codes']['delete_all'] = array(
    '#prefix' => '<div style="margin-bottom:15px">',
    '#markup' => '<a href="/admin/config/people/code-login/delete-all">Delete All Codes</a>',
    '#suffix' => '</div>',
  );

  $form['generated_codes']['send_mail'] = array(
    '#type' => 'submit',
    '#value' => t('Send Mail'),
    '#prefix' => '<div>',
    '#submit' => array('generated_codes_send_mail_submit'),
    '#suffix' => '</div>',
  );

  $form['code_login'] = array(
    '#type' => 'fieldset',
    '#title' => "General Settings",
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE,
  );

  $form['code_login']['custom_login_page_title'] = array(
    '#type' => 'textfield',
    '#title' => 'Custom Login Page Title',
    '#size' => 60,
    '#default_value' => variable_get('custom_login_page_title', 'Custom Login'),
  );


  //if($user->uid == 1) {
    
    $form['code_login']['custom_login_page_path'] = array(
      '#type' => 'textfield',
      '#title' => 'Custom Login Page Url',
      '#size' => 60,
      '#default_value' => variable_get('custom_login_page_path', 'client-login'),
      '#description' => 'Need a menu rebuild, Please clear the menu cache after saving',
    );

    $form['code_login']['code_login_redirect_page'] = array(
      '#type' => 'textfield',
      '#title' => 'Redirect Page',
      '#size' => 60,
      '#default_value' => variable_get('code_login_redirect_page', 'secure-data-room'),
    );

    $form['code_login']['expiration_start_from_first_login'] = array(
      '#type' => 'select',
      '#title' => 'Expiration Start',
      '#description' => 'Select "No" to set the expiration date on the Code creation<br />Select "Yes" to set the expiration date ot the moment of the first login<br />Default = NO',
      '#options' => array(
        'No' => 'No',
        'Yes' => 'Yes',
      ),
      '#default_value' => variable_get('expiration_start_from_first_login', 'No'),
    );
  //}

  $form['code_login']['code_login_role'] = array(
    '#type' => 'checkboxes',
    '#title' => 'Choose the role to assign',
    '#options' => $roles,
    '#default_value' => variable_get('code_login_role', array()),
  );

  $form['code_login']['code_login_mail_server'] = array(
    '#type' => 'textfield',
    '#title' => 'Choose the mail server',
    '#size' => 30,
    '#default_value' => variable_get('code_login_mail_server', ''),
    '#description' => 'Insert a mail server to automatically generate mail addresses belonging to your domain <br />(e.g. "example.com" will be generated emails with the following pattern: <br/> username@example.com<br/>You can always insert manually an email for each generated code.',
  );


  $form['code_login_errors'] = array(
    '#type' => 'fieldset',
    '#title' => "Error Messages",
    '#collapsible' => TRUE, 
    '#collapsed' => TRUE,
  );

  $form['code_login_errors']['code_login_error_not_valid'] = array(
    '#title' => t('Login code not valid'),
    '#type' => 'text_format',
    '#format' => 'filtered_html',
    '#cols' => 60,
    '#rows' => 5,
    '#default_value' => variable_get('code_login_error_not_valid', ''),
  );

  $form['code_login_errors']['code_login_error_expired'] = array(
    '#title' => t('Login code expired'),
    '#type' => 'text_format',
    '#format' => 'filtered_html',
    '#default_value' => variable_get('code_login_error_expired', ''),
  );



  $form['code_login_type'] = array(
    '#type' => 'select',
    '#title' => 'Select the validity duration of the code',
    '#options' => $types,
  );

  $form['howmany_codes'] = array(
    '#title' => t('How many codes do you want to generate?'),
    '#type' => 'textfield',
    '#default_value' => $default,
    '#size' => 5,
    '#prefix' => '',
    '#suffix' => '<span class="generate-forms-btn">Generate Code Forms</span>',
    '#ajax' => array(
      'callback' => 'ajax_codes_login_callback',
      'wrapper' => 'codes-div',
      'method' => 'replace',
      'effect' => 'fade',
    ),
  );

  $form['codes_fieldset'] = array(
    '#title' => t("Codes to Generate"),
    // The prefix/suffix provide the div that we're replacing, named by
    // #ajax['wrapper'] above.
    '#prefix' => '<div id="codes-div">',
    '#suffix' => '</div>',
    '#type' => 'fieldset',
    '#collapsible' => TRUE, 
    '#collapsed' => FALSE,
  );

  $form['separator'] = array(
      '#markup' => '<div style="width:100%; clear:both;"></div>',
  );

  $num_forms = !empty($form_state['values']['howmany_codes']) ? $form_state['values']['howmany_codes'] : 0;
  for ($i = 1; $i <= $num_forms; $i++) {
    $code = generate_random_string($length = 20);
    $form['codes_fieldset'][$code . '_fieldset'] = array(
      '#type' => 'fieldset',
      '#title' => filter_xss("Code $code"),
      '#collapsible' => TRUE, 
      '#collapsed' => TRUE,
    );

    $form['codes_fieldset'][$code . '_fieldset']['login_' . str_pad($i, 4, '0', STR_PAD_LEFT) . '_username'] = array(
      '#type' => 'textfield',
      '#title' => filter_xss("Username for the code $code"),
      '#size' => 15,
      '#default_value' => 'user_' . substr($code, 0, 6),
    );

    $form['codes_fieldset'][$code . '_fieldset']['login_' . str_pad($i, 4, '0', STR_PAD_LEFT) . '_email'] = array(
      '#type' => 'textfield',
      '#title' => filter_xss("Email for the code $code"),
      '#size' => 60,
      '#default_value' => 'user_' . substr($code, 0, 6) . '@' . $mail_server,
    );

    $form['codes_fieldset'][$code . '_fieldset']['login_' . str_pad($i, 4, '0', STR_PAD_LEFT) . '_company'] = array(
      '#type' => 'textfield',
      '#title' => filter_xss("Company Name for the code $code"),
      '#size' => 60,
      '#default_value' => '',
    );

    $form['codes_fieldset'][$code . '_fieldset']['login_' . str_pad($i, 4, '0', STR_PAD_LEFT) . '_fullname'] = array(
      '#type' => 'textfield',
      '#title' => filter_xss("Full Name of the person using the code $code"),
      '#size' => 60,
      '#default_value' => '',
    );

    $form['codes_fieldset'][$code . '_fieldset']['login_' . str_pad($i, 4, '0', STR_PAD_LEFT) . '_jobtitle'] = array(
      '#type' => 'textfield',
      '#title' => filter_xss("Job Title of the person using the code $code"),
      '#size' => 60,
      '#default_value' => '',
    );

    $form['codes_fieldset'][$code . '_fieldset']['login_' . str_pad($i, 4, '0', STR_PAD_LEFT) . '_password'] = array(
      '#type' => 'textfield',
      '#title' => filter_xss("Password for the code $code"),
      '#size' => 15,
      '#default_value' => generate_random_string($length = 15),
    );

    $form['codes_fieldset'][$code . '_fieldset']['login_' . str_pad($i, 4, '0', STR_PAD_LEFT) . '_code'] = array(
      '#type' => 'hidden',
      '#default_value' => $code,
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save / Generate'),
    '#submit' => array('code_login_submit'),
  );

  return $form;
}


function generate_random_string($length = 10) {
    return substr(str_shuffle("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"), 0, $length);
}

function code_login_form_validate($form, &$form_state) {
  $values = $form_state['values'];

  // Generate the logins values
  $logins = array();
  foreach ($values as $key => $value) {
    if (substr($key, 0 , 5) == 'login') {
      $data = explode('_', $key);
      switch ($data[2]) {
        case 'code':
          $logins[$data[1]]['code'] = $value;
          break;
        case 'username':
          $logins[$data[1]]['name'] = $value;
          break;
        case 'email':
          $logins[$data[1]]['email'] = $value;
          break;
      }    
    }
  }

  // Check if the username is already in the system
  foreach ( $logins as $key => $value) {
    $user = user_load_by_name($value['name']);
    if(!$user) {

    }
    else {      
      $code = $value['code'];
      $element = 'codes_fieldset][' . $code . '_fieldset][login_' . $key . '_username';
      form_set_error($element, 'Duplicate Username ' . $value['name']);
    }
  }
  // Check if the email is already in the system
  foreach ( $logins as $key => $value) {
    $user = user_load_by_mail($value['email']);
    if(!$user) {

    }
    else {      
      $code = $value['code'];
      $element = 'codes_fieldset][' . $code . '_fieldset][login_' . $key . '_email';
      form_set_error($element, 'Duplicate Email ' . $value['email']);
    }
  }
}

function code_login_submit($form, &$form_state) {

  $values = $form_state['values'];
  variable_set('code_login_mail_server', $values['code_login_mail_server']);
  variable_set('code_login_role', $values['code_login_role']);
  variable_set('code_login_redirect_page', $values['code_login_redirect_page']);
  variable_set('custom_login_page_title', $values['custom_login_page_title']);
  variable_set('custom_login_page_path', $values['custom_login_page_path']);
  variable_set('expiration_start_from_first_login', $values['expiration_start_from_first_login']);
  variable_set('code_login_error_not_valid', $values['code_login_error_not_valid']['value']);
  variable_set('code_login_error_expired', $values['code_login_error_expired']['value']);

  $table = CODE_LOGIN_TABLE;
  $assigned_roles = array(
    DRUPAL_AUTHENTICATED_RID => 'authenticated user',
  );
  foreach ($values['code_login_role'] as $key => $value) {
    if ($value !== 0) {
      $assigned_roles[$key] = user_role_load($key)->name;
    }
  }

  $logins = array();
  //dsm($values);
  foreach ($values as $key => $value) {
    if (substr($key, 0 , 5) == 'login') {
      $data = explode('_', $key);
      switch ($data[2]) {
        case 'username':
          $logins[$data[1]]['name'] = $value;
          break;
        case 'email':
          $logins[$data[1]]['email'] = $value;
          break;
        case 'company':
          $logins[$data[1]]['company'] = $value;
          break;
        case 'fullname':
          $logins[$data[1]]['fullname'] = $value;
          break;
        case 'jobtitle':
          $logins[$data[1]]['jobtitle'] = $value;
          break;
        case 'password':
          $logins[$data[1]]['password'] = $value;
          break;
        case 'code':
          $logins[$data[1]]['code'] = $value;
          break;      
      }
      $logins[$data[1]]['roles'] = $assigned_roles; 
      $logins[$data[1]]['type'] = (int) $values['code_login_type'];      
    }
  }
  //dsm($logins); 
  foreach ($logins as $key => $user) {
    $fields = array(
      'name' => $user['name'],
      'mail' => $user['email'],
      'pass' => $user['password'],
      'status' => 1,
      'init' => 'email address',
      'roles' => $user['roles'],
    );
   
    //the first parameter is left blank so a new user is created
    $account = user_save('', $fields);

    $expiration = '';

    if (variable_get('expiration_start_from_first_login') == 'No') {

      $types = variable_get('code_login_defined_types');

      $now = REQUEST_TIME;
      $date = date_create(date('d-m-Y', $now));
      date_add($date, date_interval_create_from_date_string($types[$user['type']]));      
      $hour = date('H',$now);
      $min = date('i',$now);
      $sec = date('s',$now);
      date_time_set($date, $hour, $min, $sec);
      $formatted_date = date_format($date, 'd-m-Y H:i:s');
      $expiration = strtotime($formatted_date);
    }

    $record = array(
      'uid' => $account->uid,
      'code' => $user['code'],    
      'code_type' => $user['type'],
      'company_name' => $user['company'],
      'full_name' => $user['fullname'],
      'job_title' => $user['jobtitle'],
      'creation' => REQUEST_TIME,
      'expiration' => $expiration,
    );
    drupal_write_record($table, $record);
    $dec_code = $user['code'];
    drupal_set_message('The code ' . $dec_code . ' has been assigned to the user ' . $user['name'], 'status');
  }
}

/**
 * Callback element needs only select the portion of the form to be updated.
 * Since #ajax['callback'] return can be HTML or a renderable array (or an
 * array of commands), we can just return a piece of the form.
 */
function ajax_codes_login_callback($form, $form_state) {
  return $form['codes_fieldset'];
}

function code_login_edit_form($form, &$form_state, $code_id) {
  $types = variable_get('code_login_defined_types');
  $types[0] = 'No Extend';
  ksort($types);
  $form = array();

  $codes_table = generate_codes_table($code_id);

  $form['current_code'] = array(
      '#markup' => $codes_table,
  );


  $form['code_login_extend'] = array(
    '#type' => 'select',
    '#title' => 'Extend the validity duration of the code',
    '#options' => $types,
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('code_login_edit_submit'),
  );
  $form['update'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#submit' => array('code_login_edit_submit'),
  );
  return $form;

}

function code_login_edit_validate($form, &$form_state) {
  switch ($values['op']) {
    case 'Cancel':
      
      break;
    case 'Update':

      break;
  }  
}

function code_login_edit_submit($form, &$form_state) {
  //dsm($form_state);
  //dsm($form);
  $values = $form_state['values'];
  switch ($values['op']) {
    case 'Cancel':
      $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
      break;
    case 'Update':
      $code_id = $form_state['build_info']['args'][0];
      if (isset($values['code_login_extend']) && $values['code_login_extend'] != 0) {
        $code = code_login_load($code_id);
        $type = $values['code_login_extend'];
        $types = variable_get('code_login_defined_types');

        $date = date_create(date('d-m-Y', $code->expiration));
        date_add($date, date_interval_create_from_date_string($types[$type]));      
        $hour = date('H',$code->expiration);
        $min = date('i',$code->expiration);
        $sec = date('s',$code->expiration);
        date_time_set($date, $hour, $min, $sec);
        $formatted_date = date_format($date, 'd-m-Y H:i:s');
        $expiration = strtotime($formatted_date);

        $record = array(
          'cid' => $code_id,  
          'uid' => $code->uid,          
          'extended' => 1,
          'extension_type' => !empty($code->extension_type) ? $code->extension_type . ',' . $type : $type,
          'expiration' => $expiration,
        );
        
        code_login_update($record);

      }
      $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
      drupal_set_message(t('The Code ' . $code_id . ' has been updated.'), 'status');
      break;
  }
}

function code_login_delete_form($form, &$form_state, $code_id) {

  $delete_code = 'Are you sure to delete the Code? This will delete also the user associated with it.';
  $form = array();

  $form['delete_code'] = array(
      '#prefix' => '<div>',
      '#markup' => $delete_code,
      '#suffix' => '</div>',
  );
  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('code_login_delete_submit'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array('code_login_delete_submit'),
  );
  return $form;

}

function code_login_delete_submit($form, &$form_state) {
  $values = $form_state['values'];

  switch ($values['op']) {
    case 'Cancel':
      $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
      break;
    case 'Delete':
      $code_id = $form_state['build_info']['args'][0];
      code_login_user_custom_delete($code_id);
      $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
      break;
  }
}


function code_login_delete_all_form($form, &$form_state) {
  $codes = db_select('login_codes','lc')
  ->fields('lc', array('cid'))
  ->execute()
  ->fetchCol();

  $codes_string = array();

  foreach ($codes as $key => $cid) {
    $code = code_login_load($cid);
    $codes_string[] = '<li>' . $code->code . ' | ' . $code->full_name . '</li>';
  }

  $delete_codes = 'Are you sure to delete all the Codes? This will delete also the users associated with them.';
  $form = array();

  $form['delete_code'] = array(
      '#prefix' => '<div>',
      '#markup' => $delete_codes,
      '#suffix' => '</div>',
  );

  $form['delete_codes'] = array(
      '#prefix' => '<div>',
      '#markup' => '<ul style="margin-bottom:45px;">' . implode('', $codes_string) . '</ul>',
      '#suffix' => '</div>',
  );

  $form['cancel'] = array(
    '#type' => 'submit',
    '#value' => t('Cancel'),
    '#submit' => array('code_login_delete_all_submit'),
  );
  $form['delete'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
    '#submit' => array('code_login_delete_all_submit'),
  );
  return $form;
}


function code_login_delete_all_submit($form, &$form_state) {
  $values = $form_state['values'];

  switch ($values['op']) {
    case 'Cancel':
      $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
      break;
    case 'Delete':  
      $codes = db_select('login_codes','lc')
      ->fields('lc', array('cid'))
      ->execute()
      ->fetchCol();

      foreach ($codes as $key => $code_id) {
        code_login_user_custom_delete($code_id);
      }
      drupal_set_message('All the Codes and the Users associated with them have been deleted.', 'status');
      $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
      break;
  }
}


function generate_codes_table($code_id = NULL) {

  if (is_null($code_id)) {
    $header = array(
        array('data' => t('Code Id'),'field' => 'cid'),
        array('data' => t('User'),'field' => 'name'),
        array('data' => t('Code'),'field' => 'code'),
        array('data' => t('Code Type'),'field' => 'code_type'),
        array('data' => t('Extended'),'field' => 'extended'),        
        array('data' => t('Extensions'),'field' => 'extension_type'),
        array('data' => t('Company'),'field' => 'company_name'),        
        array('data' => t('Full Name'),'field' => 'full_name'),    
        array('data' => t('Job Title'),'field' => 'job_title'),
        array('data' => t('Creation'),'field' => 'creation'),
        array('data' => t('Last Login'),'field' => 'login'),
        array('data' => t('Expiration'),'field' => 'expiration'),
        array('data' => t('Status')),
        array('data' => t('Extend')),
        array('data' => t('Delete')),
      );
  } else {
    $header = array(
        array('data' => t('Code Id'),'field' => 'cid'),
        array('data' => t('User'),'field' => 'name'),
        array('data' => t('Code'),'field' => 'code'),
        array('data' => t('Code Type'),'field' => 'code_type'),
        array('data' => t('Extended'),'field' => 'extended'),        
        array('data' => t('Extensions'),'field' => 'extension_type'),
        //array('data' => t('Email'),'field' => 'mail'),
        array('data' => t('Company'),'field' => 'company_name'),        
        array('data' => t('Full Name'),'field' => 'full_name'),    
        array('data' => t('Job Title'),'field' => 'job_title'),
        array('data' => t('Creation'),'field' => 'creation'),
        array('data' => t('Last Login'),'field' => 'login'),
        array('data' => t('Expiration'),'field' => 'expiration'),
        array('data' => t('Status')),
      );
  }

  $query = db_select('login_codes', 'c');
  $query->leftjoin('users', 'u', 'c.uid = u.uid');
  if (!is_null($code_id)) {
    $query->condition('cid', $code_id, '=');
  }

  $result = $query
  ->fields('c', array('cid', 'uid', 'code', 'code_type', 'extended', 'extension_type', 'company_name', 'full_name', 'job_title', 'creation', 'first_login', 'expiration'))
  ->fields('u', array('name', 'created', 'mail', 'access', 'login', 'data'));

  if (is_null($code_id)) {
    $table_sort = $result->extend('TableSort') // Add table sort extender.
      ->orderByHeader($header); // Add order by headers.

    $pager = $table_sort->extend('PagerDefault')
      ->limit(25); // 5 rows per page.

    $final = $pager->execute();
  }
  else {
    $final = $result->execute();
  }

  $types = variable_get('code_login_defined_types');
  $rows = array();
  // Looping for filling the table rows
    foreach ($final as $ord) {
      $now = time();
      if (is_null($code_id)) {
        // Fill the table rows
        $rows[$ord->cid] = array(
          $ord->cid,
          l($ord->name, 'user/'. $ord->uid .'/edit'),
          $ord->code, //l(decrypt($ord->code), CODE_LOGIN_ADMIN_PATH . '/' . $ord->cid . '/edit'),
          $types[$ord->code_type],
          $ord->extended == 0 ? 'No' : 'Yes',
          $ord->extended == 0 ? '------' : list_extensions($ord->extension_type),
          //l($ord->mail, 'mailto:' . $ord->mail),
          $ord->company_name,
          $ord->full_name,
          $ord->job_title,
          format_date($ord->creation,'custom','j/m/Y'),
          $ord->login > 0 ? format_date($ord->login,'custom','j/m/Y H:i:s') : 'Never Logged In',         
          $ord->expiration > 0 ? format_date($ord->expiration,'custom','j/m/Y') : 'Never Logged In',
          $ord->expiration > $now ? 'Valid' : 'Expired',
          l('Extend', CODE_LOGIN_ADMIN_PATH . '/' . $ord->cid . '/edit'),
          l('Delete', CODE_LOGIN_ADMIN_PATH . '/' . $ord->cid . '/delete'),
        );
      }
      else {
        // Fill the table rows
        $rows[$ord->cid] = array(
          $ord->cid,
          l($ord->name, 'user/'. $ord->uid .'/edit'),
          $ord->code, //l(decrypt($ord->code), CODE_LOGIN_ADMIN_PATH . '/' . $ord->cid . '/edit'),
          $types[$ord->code_type],
          $ord->extended == 0 ? 'No' : 'Yes',
          $ord->extended == 0 ? '------' : list_extensions($ord->extension_type),
          //l($ord->mail, 'mailto:' . $ord->mail),
          $ord->company_name,
          $ord->full_name,
          $ord->job_title,
          format_date($ord->creation,'custom','j/m/Y'),
          $ord->login > 0 ? format_date($ord->login,'custom','j/m/Y H:i:s') : 'Never Logged In',         
          $ord->expiration > 0 ? format_date($ord->expiration,'custom','j/m/Y') : 'Never Logged In',
          $ord->expiration > $now ? 'Valid' : 'Expired',
        );        
      }
    }

  $form = array();

  $form['table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('Table has no row!'),
  );

  $form['pager'] = array('#theme' => 'pager');

  return render($form);

}

function list_extensions($string) {
  $types = variable_get('code_login_defined_types');
  $extensions_array = explode(',', $string);
  $extensions = array();
  foreach ($extensions_array as $key => $extension) {
    $extensions[] = $types[$extension];
  }
  return implode(', ', $extensions);
}


function generated_codes_send_mail_submit($form, &$form_state) {
    $_SESSION['codes_login']['code_ids'] = array();
    foreach ($form_state['values']['codes_table']['table'] as $key => $value) {
      if (is_string($value)) {        
        $_SESSION['codes_login']['code_ids'][$value] = $value; 
      }
    }
    $form_state['redirect'] = 'admin/config/people/code-login/send-mail';
}


function codes_send_mail_form($form, &$form_state) {
    if (isset($_SESSION['codes_login']['code_ids'])) {
      $code_ids = $_SESSION['codes_login']['code_ids'];
    }

    $form = array();
    if (!empty($code_ids)) {
      $markup = '
      <h3>Available Tokens</h3>
      <ul>
        <li>{code} : The Login Code</li>
        <li>{username} : The username of the user related to the Code</li>
        <li>{fullname} : The full name of the user related to the Code</li>
        <li>{company} : The Company name of the user related to the Code</li>
        <li>{expiration} : The Expiration date of the Code</li>
      </ul>
      ';

      $form['code_login_form_info'] = array(
        '#markup' => $markup,
        '#prefix' => '<div id="email-info">',
        '#suffix' => '</div>',
      );
      $form['code_login_from_email'] = array(
        '#type' => 'textfield',
        '#title' => 'From Email',
        '#default_value' => variable_get('site_mail', ''),
        '#prefix' => '<div id="from-email-address">',
        '#suffix' => '</div>',
      );
      $form['code_login_email_subject'] = array(
        '#type' => 'textfield',
        '#title' => 'Subject',
        '#default_value' => variable_get('code_login_email_subject', 'Your Login Code at Tinplate Products'),
        '#prefix' => '<div id="from-email-address">',
        '#suffix' => '</div>',
      );
      $form['code_login_email_body'] = array(
        '#type' => 'textarea',
        '#title' => 'Email Body',
        '#default_value' => variable_get('code_login_email_body', ''),
        '#prefix' => '<div id="email-body">',
        '#suffix' => '</div>',
      );
      $form['send_mail'] = array(
        '#type' => 'submit',
        '#value' => t('Send Email'),
        '#submit' => array('codes_send_mail_form_submit'),
      );
    }
    else {
      $form['message'] = array(
        '#markup' => '<div style="margin-bottom:15px;">There are no Codes Selected</div>',

      );    
    }
    $form['cancel'] = array(
      '#type' => 'submit',
      '#value' => t('Cancel'),
      '#submit' => array('codes_send_mail_form_submit'),
    );
    return $form;
}

function codes_send_mail_form_submit($form, &$form_state) {
  //dsm($form_state);
  //dsm($form);
  $values = $form_state['values'];
  variable_set('code_login_from_email', $values['code_login_from_email']);
  variable_set('code_login_email_body', $values['code_login_email_body']);
  variable_set('code_login_email_subject', $values['code_login_email_subject']);
  switch ($values['op']) {
    case 'Cancel':
      $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
      break;
    case 'Send Email':

      if (isset($_SESSION['codes_login']['code_ids'])) {
        $code_ids = $_SESSION['codes_login']['code_ids'];
      }

      foreach ($code_ids as $cid => $code_id) {
        $code = code_login_load($code_id);
        //Prepare Body
        $body = $values['code_login_email_body'];
        $body = str_replace('{code}', $code->code , $body);
        $body = str_replace('{username}', $code->name , $body);
        $body = str_replace('{fullname}', $code->full_name , $body);
        $body = str_replace('{expiration}', date('l, jS F Y - H:i:s', $code->expiration) , $body);
        $body = str_replace('{company}', $code->company_name , $body);
        // Prepare Subject
        $subject = $values['code_login_email_subject'];
        //Prepare Params
        $params = array(
          'body' => $body, 
          'subject' => $subject,
        );
        // Prepare Mail
        $key = 'Login Code';
        $to = $code->mail; 
        $from = $values['code_login_from_email'];
        $mail = drupal_mail('code_login', $key, $to, language_default(), $params, $from);
        drupal_set_message('A mail have been sent to ' . $code->mail, 'status' );
      }
    $form_state['redirect'] = CODE_LOGIN_ADMIN_PATH;
    break;
  }
}

/**
 * Implements hook_mail()
 * @param 
 *   type $key to decide which email body to sent on basis of key parameter inacese of multiple email content
 * @param 
 *   type $message the email content to be sent.Message array contains 'subject and body ' for the email.
 * @param 
 *   type $params using to get the cusotm email content from a function.This can be used in my other ways aslo as per need.
 */
function code_login_mail($key, &$message, $params) {
  $language = $message['language'];
  $headers = array(
    'MIME-Version' => '1.0',
    'Content-Type' => 'text/html; charset=UTF-8; format=flowed',
    'Content-Transfer-Encoding' => '8Bit',
    'X-Mailer' => 'Drupal'
  );
  switch ($key) {
//switching on $key lets you create variations of the email based on the $key parameter
    case 'Login Code':
    /*
      foreach ($headers as $key => $value) {
        $message['headers'][$key] = $value;
      }
      */
      $message['subject'] = $params['subject'];
//the email body is here, inside the $message array
      $message['body'][] = $params['body'];
      break;
  }
}
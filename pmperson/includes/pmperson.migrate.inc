<?php
/**
 * @file
 * pmperson.migrate.inc
 */

define('PMPERSON_MIGRATE_MAX_JOB_PER_BATCH', 2);

/**
 * Helper function for migrating between PM Person nodes and Drupal users.
 */
function pmperson_migrate(&$sandbox) {
  _pmperson_migrate_create_pmperson_node_type();
  // Create a new Drupal user for PM Person nodes that are not associated
  // with an existing Drupal user.
  if (empty($sandbox['pmperson_current_nid_of_user_being_created'])) {
    $sandbox['pmperson_current_nid_of_user_being_created'] = 0;
  }
  if (pmperson_migrate_create_users_if_required($sandbox) == FALSE) {
    $sandbox['#finished'] = 0;
    return;
  }

  // Create and attach fields to Drupal's user account.
  if (pmperson_migrate_create_fields() == FALSE) {
    $sandbox['#finished'] = 0.25;
    return;
  }

  // Migrate data from pmperson node to Drupal user account.
  if (empty($sandbox['pmperson_current_nid_of_field_being_migrated'])) {
    $sandbox['pmperson_current_nid_of_field_being_migrated'] = 0;
  }
  if (pmperson_migrate_field_data($sandbox) == FALSE) {
    $sandbox['#finished'] = 0.5;
    return;
  }

  // Delete all pmperson nodes.
  if (empty($sandbox['pmperson_current_nid_of_node_being_deleted'])) {
    $sandbox['pmperson_current_nid_of_node_being_deleted'] = 0;
  }
  if (pmperson_migrate_content_kill($sandbox) == FALSE) {
    $sandbox['#finished'] = 0.75;
    return;
  }
  // Delete pmperson node type.
  node_type_delete('pmperson');

  // @todo pmperson table is required by other modules using user reference.
  // Delete it when all the submodules have been ported.
  // db_drop_table('pmperson');

  // Delete variables previously set by the PM Person module (now redundant).
  variable_del('node_options_pmperson');
  variable_del('node_permissions_pmperson');
  $sandbox['#finished'] = 1;
  return 'PM Person nodes have been migrated to fields on Drupal users.';
}

/**
 * Delete all pmperson nodes.
 */
function pmperson_migrate_content_kill(&$sandbox) {
  $results = db_select('node', 'n')
    ->fields('n', array('nid'))
    ->condition('type', 'pmperson')
    ->condition('nid', $sandbox['pmperson_current_nid_of_node_being_deleted'], '>')
    ->range(0, PMPERSON_MIGRATE_MAX_JOB_PER_BATCH)
    ->execute();
  $count = 0;
  foreach ($results as $result) {
    $count++;
    $nids[] = $result->nid;
    $sandbox['pmperson_current_nid_of_node_being_deleted'] = $result->nid;
  }

  if (!empty($nids)) {
    node_delete_multiple($nids);
  }
  return empty($count);
}

/**
 * Recreate node type for migration (hook_node_info has already been deleted).
 */
function _pmperson_migrate_create_pmperson_node_type() {
  $names = node_type_get_names();
  if (empty($names['pmperson'])) {
    $type = array(
      'type' => 'pmperson',
      'name' => 'Person',
      'base' => 'pmperson',
      'description' => 'A person for Project Management.',
      'title_label' => 'Name',
      'body_label' => 'Description',
    );
    $content_type = node_type_set_defaults($type);
    node_type_save($content_type);
  }
}

/**
 * Checks for conflicts that would affect the running of update.php.
 */
function pmperson_migrate_update_could_be_performed() {
  if (db_table_exists('pmperson')) {
    $counts = pmperson_migrate_email_missmatch_count();
    if ($counts > 0) {
      return FALSE;
    }
  }
  return TRUE;
}

/**
 * Gets count of email conflicts between pmperson nad drupal user.
 */
function pmperson_migrate_email_missmatch_count() {
  $results = db_query(" SELECT pm.nid FROM pmperson as pm
                        LEFT JOIN users as u
                        ON pm.user_uid = u.uid
                        WHERE pm.email <> u.mail AND u.mail <> ''");
  $count = $results->rowCount();

  $results = db_query(" SELECT pm.nid FROM pmperson as pm
                        LEFT JOIN users as u
                        ON pm.user_uid <> u.uid
                        WHERE pm.email = u.mail AND pm.user_uid = '0'");

  $count += $results->rowCount();
  return $count;
}

/**
 * Page callback for PM_PMPERSON_RESOLVE_DEPENDENCIES_LINK.
 *
 * @see pmperson_menu()
 * @todo Need to word this better Issue #1412134-84
 */
function pmperson_migrate_page_callback() {
  if (pmperson_migrate_update_could_be_performed()) {
    $link = l('update script', 'update.php');
    return t('No conflicts here, PM Person migration could be performed.
      If you came here to resolve migration issue, you can go back to !link.',
      array('!link' => $link)
    );
  }

  $form_fix_1 = drupal_get_form('pmperson_migrate_fix_existing_users_form');

  $render = array(
    'preface' => array(
      '#markup' => "
        <p>
        - The information captured by legacy PM Person nodes is now incorporated
        into user accounts.
        </p><p>
        - This page will ensure that all PM Person nodes can be uniquely matched
         to existing users so that the data migration can take place.
        </p>
      ",
    ),
    'tabel_1_info' => array(
      '#markup' => "
        <b>Suggested matches</b>
        <p>
        This table shows PM Person nodes with the same email address as a user
        account.
        </p><p>
        Normally these can be matched automatically by clicking the button below.
        Remaining PM Person nodes will need to be manually matched.
        </p>
      ",
    ),
    'table_issue_1' => _pmperson_migrate_get_conflicting_email_with_existing_users_table(),
    'try_fix_1' => array('#type' => 'markup', '#markup' => render($form_fix_1)),
    'tabel_2_info' => array(
      '#markup' => "
        <hr><br>
        <b>Conflicts</b>
        <p>
        This table shows PM Person nodes where a match cannot be made
        automatically due to data differences between them.
        </p><p>
        Edit the PM Person node or user accounts until there are no remaining
        conflicts.
        </p>
      ",
    ),
    'table_issue_2' => _pmperson_migrate_get_conflicting_email_with_attached_users_table(),
  );
  return $render;
}

/**
 * Form callback for automatching Drupal users to PM Persons.
 */
function pmperson_migrate_fix_existing_users_form($form, $form_state) {
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Adjust pmperson nodes'),
    '#prefix' => '<br>',
    '#suffix' => t('Automatically match Drupal user with pmperson node using
      email if Drupal user is not associated with any other pmperson node.'),
  );
  return $form;
}

/**
 * Form submit function for automatching Drupal users to PM Persons.
 */
function pmperson_migrate_fix_existing_users_form_submit($form, $form_state) {
  $unmapped_results = db_select('pmperson', 'pmp')
    ->fields('pmp')
    ->condition('pmp.user_uid', '0')
    ->execute();
  foreach ($unmapped_results as $record) {
    if ($account = user_load_by_mail($record->email)) {
      if (_pmperson_migrate_get_pmperson_from_uid($account->uid) == FALSE) {
        _pmperson_migrate_associate_user_with_pmperson($record, $account);
      }
    }
  }
}

/**
 * Get list of pmnodes that has conflicting email with the attached drupal user.
 */
function _pmperson_migrate_get_conflicting_email_with_attached_users_table() {
  $header = array(
    'nid'   => array('data' => t('PM Person Node ID'),  'field' => 'nid'),
    'email' => array('data' => t('PM Person E-Mail'),    'field' => 'email'),
    'mail'  => array('data' => t('Drupal User E-Mail'),  'field' => 'mail'),
    'uid'   => array('data' => t('Drupal User ID'),    'field' => 'uid'),
    'name'  => array('data' => t('Drupal User name'),   'field' => 'name'),
  );

  if (db_table_exists('pmperson')) {
    $query = db_select('pmperson', 'p')
      ->extend('PagerDefault')
      ->limit(10);

    $query->join('users', 'u', 'p.user_uid = u.uid AND p.email <> u.mail');

    $query->fields('p', array(
          'nid',
          'email',
        )
      )
      ->fields('u', array(
          'mail',
          'uid',
          'name',
        )
      )
      ->condition('u.uid', '', '!=');

    $query = $query->extend('TableSort')
          ->orderByHeader($header);
    $result = $query->execute()->fetchAllAssoc('nid');
  }
  else {
    $result = array();
  }

  $render = array(
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => array_map(function ($row) {
        return _pmperson_migrate_adjust_row($row);
      }, $result),
      '#sticky' => TRUE,
      '#caption' => t('Conflicts.'),
      '#empty' => t('No issues found.'),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
  );

  return $render;
}
/**
 * Get list of pmnodes that has conlficting email with existing Drupal user.
 */
function _pmperson_migrate_get_conflicting_email_with_existing_users_table() {
  $header = array(
    'nid'   => array('data' => t('PM Person Node ID'),  'field' => 'nid'),
    'email' => array('data' => t('PM Person email'),    'field' => 'email'),
    'mail'  => array('data' => t('Drupal User email'),  'field' => 'mail'),
    'uid'   => array('data' => t('Drupal User uid'),    'field' => 'uid'),
    'name'  => array('data' => t('Drupal User name'),   'field' => 'name'),
  );

  if (db_table_exists('pmperson')) {
    $query = db_select('pmperson', 'p')
      ->extend('PagerDefault')
      ->limit(10);
    $query->join('users', 'u', 'p.email = u.mail AND p.user_uid <> u.uid');
    $query->fields('p', array(
          'nid',
          'email',
        )
      )
      ->fields('u', array(
          'mail',
          'uid',
          'name',
        )
      )
      ->condition('p.user_uid', '0');

    $query = $query->extend('TableSort')
          ->orderByHeader($header);
    $result = $query->execute()->fetchAllAssoc('nid');
  }
  else {
    $result = array();
  }

  $render = array(
    'table' => array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => array_map(function ($row) {
        return _pmperson_migrate_adjust_row($row);
      }, $result),
      '#sticky' => TRUE,
      '#caption' => t("Suggested Matches."),
      '#empty' => t('No issues found.'),
    ),
    'pager' => array(
      '#theme' => 'pager',
    ),
  );

  return $render;
}
/**
 * Custom function to map row values to display values.
 *
 * @see pmperson_migrate_page_callback()
 */
function _pmperson_migrate_adjust_row($row, $show_form = TRUE) {
  $query_string = array('destination' => PMPERSON_RESOLVE_DEPENDENCIES_LINK);
  $operations = array(
    'view' => array(
      'title' => $row->uid,
      'href' => 'user/' . $row->uid,
      'html' => TRUE,
      'query' => $query_string,
    ),
    'edit' => array(
      'title' => t('(edit'),
      'href' => 'user/' . $row->uid . '/edit',
      'html' => TRUE,
      'query' => $query_string,
    ),
    'delete' => array(
      'title' => t('delete)'),
      'href' => 'user/' . $row->uid . '/cancel',
      'html' => TRUE,
      'query' => $query_string,
    ),
  );
  $user_links = theme('links', array(
    'links' => $operations,
    'attributes' => array('class' => array('links', 'inline')),
  ));
  if ($show_form) {
    $user_email_form = drupal_get_form("pmperson_migrate_email_adjust_form_user_{$row->uid}", 'user', $row->uid, $row->mail);
    $pmperson_email_form = drupal_get_form("pmperson_migrate_email_adjust_form_pmperson_{$row->nid}", 'pmperson', $row->nid, $row->email);
    $out = array(
      'nid' => $row->nid,
      'email' => drupal_render($pmperson_email_form),
      'mail' => drupal_render($user_email_form),
      'uid' => $row->uid ? $user_links : '',
      'name' => $row->name,
    );
  }
  else {
    $out = array(
      'nid' => $row->nid,
      'email' => $row->email,
      'mail' => $row->mail,
      'uid' => $row->uid ? $user_links : '',
      'name' => $row->name,
    );
  }
  return $out;
}

/**
 * Creates Drupal User and associate it with pmperson node.
 */
function pmperson_migrate_create_users_if_required(&$sandbox) {
  $unmapped_results = db_select('pmperson', 'pmp')
    ->fields('pmp')
    ->condition('pmp.user_uid', '0')
    ->condition('nid', $sandbox['pmperson_current_nid_of_user_being_created'], '>')
    ->range(0, PMPERSON_MIGRATE_MAX_JOB_PER_BATCH)
    ->execute();

  $count = 0;
  foreach ($unmapped_results as $record) {
    $count++;
    $account = NULL;
    if ($account = user_load_by_mail($record->email)) {
      // If the email is already associated with another account, create new.
      // and change the current email.
      if (_pmperson_migrate_get_pmperson_from_uid($account->uid)) {
        $account = _pmperson_migrate_create_user($record);
        db_update('pmperson')
          ->fields(array(
            'email' => $account->mail,
          ))
          ->condition('user_uid', $account->uid)
          ->execute();
      }
    }
    else {
      $account = _pmperson_migrate_create_user($record);
    }
    _pmperson_migrate_associate_user_with_pmperson($record, $account);
  }
  return empty($count);
}

/**
 * Associate user with newly created pmperson acount.
 */
function _pmperson_migrate_associate_user_with_pmperson($record, $account) {
  // If $account is empty or is for anonymous user.
  if (!$record OR !$record->nid OR !$account OR !$account->uid) {
    watchdog('pmperson', 'message', array('record' => $record), WATCHDOG_NOTICE);
    return FALSE;
  }
  db_update('pmperson')
    ->fields(array('user_uid' => $account->uid, 'email' => $account->mail))
    ->condition('nid', $record->nid)
    ->execute();
  return TRUE;
}

/**
 * Load pmperson associated with a Drupal user.
 */
function _pmperson_migrate_get_pmperson_from_uid($uid) {
  $record = db_select('pmperson', 'pmp')
    ->fields('pmp')
    ->condition('pmp.user_uid', $uid)
    ->execute()
    ->fetchAssoc();
  if ($record AND $node = node_load($record['nid'])) {
    return $node;
  }
  return FALSE;
}

/**
 * Create a Drupal User.
 */
function _pmperson_migrate_create_user($record) {
  $account = FALSE;
  $node = node_load($record->nid);
  if ($node) {
    $mail = $record->email;
    if (empty($mail) OR user_load_by_mail($record->email)) {
      $mail = 'pmperson_' . $node->nid . '@example.com';
    }

    $name = check_plain($node->title);
    $name = _pmperson_migrate_generate_unique_username($name, TRUE, array($mail));
    $account = user_save(NULL, array(
              'name' => $name,
              'mail' => $mail,
              'init' => $mail,
              'pass' => user_password(),
            ));
  }
  return $account;
}


/**
 * A custom function to generate unique username.
 */
function _pmperson_migrate_generate_unique_username($username, $reset = FALSE, $tries = array()) {
  static $suffix = 1;
  if ($reset) {
    $suffix = 1;
  }
  if ($suffix < 2) {
    $duplicate = _pmperson_migrate_check_if_username_exists($username);
    if ($duplicate == FALSE) {
      return $username;
    }
  }
  else {
    $duplicate = _pmperson_migrate_check_if_username_exists("$username $suffix");
  }
  if (!empty($duplicate) AND !empty($tries)) {
    foreach ($tries as $try) {
      if (_pmperson_migrate_check_if_username_exists($try) == FALSE) {
        return $try;
      }
    }
  }
  // Loop until username is valid.
  if (!empty($duplicate)) {
    $suffix++;
    _pmperson_migrate_generate_unique_username($username);
  }
  // Add number at end of username if it already exists.
  $username = ($suffix < 2) ? $username : "$username $suffix";
  return $username;
}

/**
 * Checks whether a given username exists.
 */
function _pmperson_migrate_check_if_username_exists($username) {
  $result = db_query('SELECT u.uid FROM {users} u WHERE name = :name', array(':name' => $username));
  if ($result) {
    foreach ($result as $r) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Creates and attaches fields to Drupal user.
 */
function pmperson_migrate_create_fields(&$sandbox) {
  module_load_include('inc', 'pmperson', 'includes/pmperson.field_base');
  module_load_include('inc', 'pmperson', 'includes/pmperson.field_instance');
  module_load_include('inc', 'pm', 'includes/pm.field');

  $field_bases = pmperson_default_field_bases();
  pm_field_bases_create_if_required($field_bases);

  $field_instances = pmperson_default_field_instances();
  pm_field_instances_create_if_required($field_instances);
  return TRUE;
}

/**
 * Migrate pmperson node fields data to drupal user bundle.
 */
function pmperson_migrate_field_data(&$sandbox) {
  $results = db_select('pmperson', 'pmp')
    ->fields('pmp')
    ->condition('nid', $sandbox['pmperson_current_nid_of_field_being_migrated'], '>')
    ->range(0, PMPERSON_MIGRATE_MAX_JOB_PER_BATCH)
    ->execute();
  $count = 0;
  foreach ($results as $pmperson) {
    $count++;
    $sandbox['pmperson_current_nid_of_field_being_migrated'] = $pmperson->nid;
    _pmperson_migrate_field_data($pmperson);
  }
  return empty($count);
}

/**
 * Helper function.
 *
 * @see pmperson_migrate_field_data()
 */
function _pmperson_migrate_field_data($pmperson) {
  $field_mapping = array(
    'im' => 'pmperson_im',
    'email' => 'pmperson_mail',
    'phone' => 'pmperson_phone',
    'prefix' => 'pmperson_prefix',
  );
  try {
    $account = user_load($pmperson->user_uid);

    $title = _pmperson_migrate_node_get_title($pmperson->nid);
    $edit['pmperson_name'][LANGUAGE_NONE][0]['value'] = $title;

    $body = _pmperson_migrate_node_get_body($pmperson->nid);
    $edit['pm_description'][LANGUAGE_NONE][0]['value'] = $body;

    foreach ($field_mapping as $key => $field_name) {
      if ($pmperson->$key) {
        $edit[$field_name][LANGUAGE_NONE][0]['value'] = $pmperson->$key;
      }
    }
    $edit['pmperson_www'][LANGUAGE_NONE][0]['url'] = $pmperson->www;
    $edit['pmperson_www'][LANGUAGE_NONE][0]['title'] = $pmperson->www;
    user_save($account, $edit);

  }
  catch (EntityMetadataWrapperException $exc) {
    watchdog(
      'pmperson',
      'See ' . __FUNCTION__ . '() ' . $exc->getTraceAsString(),
      NULL, WATCHDOG_ERROR
    );
  }
}

/**
 * A quick function to load node title without loading the whole node.
 */
function _pmperson_migrate_node_get_title($nid) {
  return db_query('SELECT title FROM {node} WHERE nid = :nid', array(':nid' => $nid))->fetchField();
}

/**
 * A quick function to load node body without loading the whole node.
 */
function _pmperson_migrate_node_get_body($nid) {
  return $v = db_query('SELECT body_value FROM {field_data_body} WHERE entity_id = :nid AND entity_type = :entity_type', array(':nid' => $nid, ':entity_type' => 'node'))->fetchField();
}

/**
 * Form for resolving email conflicts.
 *
 * @see pmperson_forms()
 */
function pmperson_migrate_email_adjust_form($form, $form_state, $type = NULL, $id = NULL, $mail = NULL) {
  $form = array();
  if ($id) {
    $form['type'] = array(
      '#type' => 'value',
      '#value' => $type,
    );
    $form['id'] = array(
      '#type' => 'value',
      '#value' => $id,
    );
    $description = '';
    switch ($type) {
      case 'user':
        $nid = db_query('SELECT nid FROM {pmperson} WHERE user_uid = :user_uid', array(':user_uid' => $id))->fetchField();
        $description = 'linked to pmperson: ' . _pmperson_migrate_node_get_title($nid) . " (nid: $nid)";
        break;

      case 'pmperson':
        $uid = db_query('SELECT user_uid FROM {pmperson} WHERE nid = :nid', array(':nid' => $id))->fetchField();
        if ($uid AND $account = user_load($uid)) {
          $description = 'linked to user: ' . $account->name . " (uid: {$account->uid})";
        }
        else {
          $description = t('unlinked');
        }
      default:
        // @todo Code...
        break;

    }
    $form['mail'] = array(
      '#type' => 'textfield',
      '#default_value' => $mail,
      '#description' => $description,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Save'),
      '#submit' => array('pmperson_migrate_email_adjust_form_save_submit'),
      '#validate' => array('pmperson_migrate_email_adjust_form_save_validate'),
    );
    $form['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('pmperson_migrate_email_adjust_form_delete_submit'),
    );
  }
  return $form;
}

/**
 * Validation function which is applicable for save operation only.
 *
 * @see pmperson_migrate_email_adjust_form()
 */
function pmperson_migrate_email_adjust_form_save_validate($form, $form_state) {
  $id   = $form_state['values']['id'];
  $type = $form_state['values']['type'];
  $mail = $form_state['values']['mail'];
  switch ($type) {
    case 'user':
      if (valid_email_address($mail)) {
        $account = user_load_by_mail($mail);
        if ($account AND $account->uid != $id) {
          form_set_error('mail', t('User already exists with the given E-mail'));
        }
      }
      else {
        form_set_error('mail', t('E-mail address provided is not valid'));
      }
      break;

    case 'pmperson':
      if ($mail AND !valid_email_address($mail)) {
        form_set_error('mail', t('E-mail address provided is not valid'));
      }
      break;

  }
}

/**
 * Save submit handler.
 *
 * @see pmperson_migrate_email_adjust_form()
 */
function pmperson_migrate_email_adjust_form_save_submit($form, $form_state) {
  $id   = $form_state['values']['id'];
  $type = $form_state['values']['type'];
  $mail = $form_state['values']['mail'];
  switch ($type) {
    case 'user':
      $account = user_load($id);
      $edit['mail'] = $mail;
      $edit['init'] = $mail;
      user_save($account, $edit);
      break;

    case 'pmperson':
      db_update('pmperson')
        ->fields(array(
          'email' => $mail,
        ))
        ->condition('nid', $id)
        ->execute();
      break;

  }
}

/**
 * Delete submit handler.
 *
 * @see pmperson_migrate_email_adjust_form()
 */
function pmperson_migrate_email_adjust_form_delete_submit($form, &$form_state) {
  $id = $form_state['values']['id'];
  $type = $form_state['values']['type'];
  switch ($type) {
    case 'user':
      if ($id != 0 AND $id != 1) {
        // It is safer to use drupal_goto() as it simulates a link.
        // This avoids an unintended form submission.
        $query_string = array('destination' => PMPERSON_RESOLVE_DEPENDENCIES_LINK);
        drupal_goto("user/$id/cancel", array('query' => $query_string));
      }
      break;

    case 'pmperson':
      db_delete('pmperson')
        ->condition('nid', $id)
        ->execute();
      break;

  }
}

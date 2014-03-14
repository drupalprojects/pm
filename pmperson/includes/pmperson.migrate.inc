<?php
/**
 * @file
 * pmperson.migrate.inc
 */

/**
 * Helper function for migrating between PM Person nodes and Drupal users.
 */
function pmperson_migrate(&$sandbox) {
  // Create a new Drupal user for PM Person nodes that are not associated
  // with an existing Drupal user.
  pmperson_migrate_create_users_if_required($sandbox);

  // Create and attach fields to Drupal's user account.
  pmperson_migrate_create_fields($sandbox);

  // Migrate data from pmperson node to Drupal user account.
  pmperson_migrate_field_data($sandbox);

  // Delete all pmperson nodes.
  node_type_delete('pmperson');

  // TODO: How to handle cleanup since data is probably required by other
  // modules especially for mapping references.
  // db_drop_table('pmperson');

  // Delete variables previously set by the PM Person module that are now redundant.
  variable_del('node_options_pmperson');
  variable_del('node_permissions_pmperson');

  return 'PM Person nodes have been migrated to fields on Drupal users.';
}

/**
 * Checks the database and see if there are any conflicts that would
 * affect the update.
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
 * page callback for PM_PMPERSON_RESOLVE_DEPENDENCIES_LINK
 * @see: pmperson_menu().
 */
function pmperson_migrate_page_callback() {
  $form = drupal_get_form('pmperson_migrate_fix_existing_users_form');
  $render = array(
    'preface' => array(
      '#markup' => "
        <p>
        - The information captured by legacy PM Person nodes is now incorporated into user accounts.
        </p><p>
        - This page will ensure that all PM Person nodes can be uniquely matched to existing users so that the data migration can take place.
        </p>
      "
    ),
    'tabel_1_info' => array(
      '#markup' => "
        <b>Suggested matches</b>
        <p>
        This table shows PM Person nodes with the same email address as a user account.
        Normally these can be matched automatically by clicking the button below. Remaining PM Person nodes will need to be manually matched.
        </p>
      ",
    ),
    'table_issue_1' => _pmperson_migrate_get_conflicting_email_with_existing_users_table(),
    'try_fix' => array('#type' => 'markup', '#markup' => render($form)),
    'tabel_2_info' => array(
      '#markup' => "
        <hr><br>
        <b>Conflicts</b>
        <p>
        This table shows PM Person nodes where a match cannot be made automatically due to data differences between them.
        Edit the PM Person node or user accounts until there are no remaining conflicts.
        </p>
      ",
    ),
    'table_issue_2' => _pmperson_migrate_get_conflicting_email_with_attached_users_table(),
  );
  return $render;
}

function pmperson_migrate_fix_existing_users_form($form, $form_state) {
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Adjust pmperson nodes'),
    '#prefix' => '<br>',
    '#suffix' => t('Automatically match Drupal user with pmperson node using email if Drupal user is not associated with any other pmperson node.'),
  );
  return $form;
}

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
 * @see: pmperson_migrate_page_callback().
 */
function _pmperson_migrate_adjust_row($row, $show_form = TRUE) {
  $query_string = array('destination' => PMPERSON_RESOLVE_DEPENDENCIES_LINK);
  $operations = array(
    'view' => array(
      'title' => $row->uid,
      'href' => 'user/' . $row->uid,
      'html' => true,
      'query' =>  $query_string,
    ),
    'edit' => array(
      'title' => t('(edit'),
      'href' => 'user/' . $row->uid . '/edit',
      'html' => true,
      'query' =>  $query_string,
    ),
    'delete' => array(
      'title' => t('delete)'),
      'href' => 'user/' . $row->uid . '/cancel',
      'html' => true,
      'query' =>  $query_string,
    ),
  );
  $user_links =  theme('links', array(
    'links' => $operations,
    'attributes' => array('class' => array('links', 'inline')),
  ));
  if ($show_form) {
    $user_email_form     = drupal_get_form("pmperson_migrate_email_adjust_form_user_{$row->uid}", 'user', $row->uid, $row->mail);
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
function pmperson_migrate_create_users_if_required() {
  $unmapped_results = db_select('pmperson', 'pmp')
    ->fields('pmp')
    ->condition('pmp.user_uid', '0')
    ->execute();
  foreach ($unmapped_results as $record) {
    $account = NULL;
    if ($account = user_load_by_mail($record->email)) {
      // If the email is already associated with another account, create new.
      if (_pmperson_migrate_get_pmperson_from_uid($account->uid)) {
        $account = _pmperson_migrate_create_user($record);
      }
    }
    else {
      $account = _pmperson_migrate_create_user($record);
    }
    _pmperson_migrate_associate_user_with_pmperson($record, $account);
  }
}

/**
 * Associate user with newly created pmperson acount.
 */
function _pmperson_migrate_associate_user_with_pmperson($record, $account) {
  // if $account is empty or is for anonymous user.
  if (!$record OR !$record->nid OR !$account OR !$account->uid) {
    watchdog('pmperson', 'message', array('record' => $record), WATCHDOG_NOTICE);
    return FALSE;
  }
  db_update('pmperson')
    ->fields(array('user_uid' => $account->uid))
    ->condition('nid', $record->nid)
    ->execute();
  return TRUE;
}

/**
 * load pmperson associated with a Drupal user.
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
    $mail = 'pmperson_' . $node->nid . '@example.com';
    $name = check_plain($node->title);
    $account = user_save(array(
              'name' => $name,
              'mail' => $mail,
              'init' => $mail,
              'pass' => user_password(),
            ));
  }
  return $account;
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
}

/**
 * Migrate pmperson node fields data to drupal user bundle.
 */
function pmperson_migrate_field_data(&$sandbox) {
  $results = db_select('pmperson', 'pmp')
    ->fields('pmp')
    ->execute();
  foreach ($results as $pmperson) {
     _pmperson_migrate_field_data($pmperson);
  }
}

/**
 * Helper function.
 * @see: pmperson_migrate_field_data().
 */
function _pmperson_migrate_field_data($pmperson) {
  $field_mapping = array(
    'im' => 'field_pm_user_im',
    'email' => 'field_pm_user_mail',
    'phone' => 'field_pm_user_phone',
    'prefix' => 'field_pm_user_prefix',
    'www' => 'field_pm_user_www',
  );
  $title_field = 'field_pm_user_name';
  $body_field = 'field_pm_description';
  try {
    $account = user_load($pmperson->user_uid);
    $wrapper = entity_metadata_wrapper('user', $account );
    $title = _pmperson_migrate_node_get_title($pmperson->nid);
    $wrapper->$title_field->set($title);
    $body =  _pmperson_migrate_node_get_body($pmperson->nid);
    $wrapper->$body_field->set($body);

    foreach ($field_mapping as $key => $field_name) {
       $wrapper->$field_name->set($pmperson->$key);
    }
    $wrapper->save();

  } catch (EntityMetadataWrapperException $exc) {
    watchdog(
      'pmperson',
      'See '  . __FUNCTION__ . '() ' .  $exc->getTraceAsString(),
       NULL, WATCHDOG_ERROR
    );
  }
}

/**
 * A quick function to load node title without loading the whole node.
 */
function _pmperson_migrate_node_get_title($nid) {
  return db_query('SELECT title FROM {node} WHERE nid = :nid',array(':nid' =>$nid))->fetchField();
}

/**
 * A quick function to load node body without loading the whole node.
 */
function _pmperson_migrate_node_get_body($nid) {
  return $v = db_query('SELECT body_value FROM {field_data_body} WHERE entity_id = :nid AND entity_type = node',array(':nid' =>$nid))->fetchField();
}

/**
 * Form for resolving email conflicts.
 * @see pmperson_forms().
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
        $nid = db_query('SELECT nid FROM {pmperson} WHERE user_uid = :user_uid',array(':user_uid' =>$id))->fetchField();
        $description = 'linked to pmperson: ' . _pmperson_migrate_node_get_title($nid) . "(nid: $nid)";
        break;
      case 'pmperson':
        $uid = db_query('SELECT user_uid FROM {pmperson} WHERE nid = :nid',array(':nid' =>$id))->fetchField();
        if($uid AND $account = user_load($uid)) {
          $description = 'linked to user: ' . $account->name;
        }
        else {
          $description = t('unlinked');
        }
      default:
        # code...
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
 * @see  pmperson_migrate_email_adjust_form
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
 * @see  pmperson_migrate_email_adjust_form
 */
function pmperson_migrate_email_adjust_form_save_submit($form, $form_state) {
  $id   = $form_state['values']['id'];
  $type = $form_state['values']['type'];
  $mail = $form_state['values']['mail'];
  switch ($type) {
    case 'user':
      $account = user_load($id);
      $edit['mail'] = $mail;
      user_save($account, $edit);
      break;
    case 'pmperson':
      $num_updated = db_update('pmperson')
        ->fields(array(
          'email' => $mail
        ))
        ->condition('nid', $id)
        ->execute();
      break;
  }
}

/**
 * Delete submit handler.
 * @see  pmperson_migrate_email_adjust_form
 */
function pmperson_migrate_email_adjust_form_delete_submit($form, &$form_state) {
  $id   = $form_state['values']['id'];
  $type = $form_state['values']['type'];
  switch ($type) {
    case 'user':
      if ($id != 0 AND $id != 1) {
        // user_delete($id);
        // safe to use drupal_goto in submit handler cause we want to simulate a link.
        // instead of a form submission.
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

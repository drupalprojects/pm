<?php
/**
 * @file
 * pmperson.migrate.inc
 */

/**
 * Migrate helper for pmperson module from D6 to D7.
 */
function pmperson_migrate(&$sandbox) {
  // create drupal users for unassociated drupal account and associate it with
  // pmperson
  pmperson_migrate_create_users_if_required($sandbox);

  // Create and attach fields to Drupal's user account.
  pmperson_migrate_create_fields($sandbox);

  // Migrate data from pmperson node to Drupal user account.
  pmperson_migrate_field_data($sandbox);
  // TODO: How to handle cleanup since data is probably required by other
  // modules.
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
   'table_issue_1' => _pmperson_migrate_get_conflicting_email_with_attached_users_table(),
   'table_issue_2' => _pmperson_migrate_get_conflicting_email_with_existing_users_table(),
   'try_fix' => array('#type' => 'markup', '#markup' => render($form)),
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
 * Get list of pmnodes that has conlficting email with the attached drupal user.
 */
function _pmperson_migrate_get_conflicting_email_with_attached_users_table() {
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
      '#caption' => t('PM PERSON EMAIL and DRUPAL USER EMAIL, should match before any updates could be attempted. Edit either of the entity and resolve the issue.'),
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
      '#caption' => t("List of PM Person nodes that are not linked to Drupal User account with same email."),
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
function _pmperson_migrate_adjust_row($row) {
  $operations = array(
    'edit' => array(
      'title' => t('(edit'),
      'href' => 'node/' . $row->nid . '/edit',
    ),
    'delete' => array(
      'title' => t('delete)'),
      'href' => 'node/' . $row->nid . '/delete',
    ),
    'view' => array(
      'title' => $row->nid,
      'href' => 'node/' . $row->nid,
    ),
  );
  $pmperson_links =  theme('links', array(
    'links' => $operations,
    'attributes' => array('class' => array('links', 'inline')),
  ));

  $operations = array(
    'edit' => array(
      'title' => t('(edit'),
      'href' => 'user/' . $row->uid . '/edit',
    ),
    'delete' => array(
      'title' => t('delete)'),
      'href' => 'user/' . $row->uid . '/delete',
    ),
    'view' => array(
      'title' => $row->uid,
      'href' => 'user/' . $row->uid,
    ),
  );
  $user_links =  theme('links', array(
    'links' => $operations,
    'attributes' => array('class' => array('links', 'inline')),
  ));

  $out = array(
    'nid' => $pmperson_links,
    'email' => $row->email,
    'mail' => $row->mail,
    'uid' => $row->uid ? $user_links : '',
    'name' => $row->name,
  );
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
      'MODULE_NANE',
      'See '  . __FUNCTION__ . '() ' .  $exc->getTraceAsString(),
       NULL, WATCHDOG_ERROR
    );
  }
}

/**
 * A quick funciton to load node title without loading the whole node.
 */
function _pmperson_migrate_node_get_title($nid) {
  return db_query('SELECT title FROM {node} WHERE nid = :nid',array(':nid' =>$nid))->fetchField();
}

/**
 * A quick funciton to load node title without loading the whole node.
 */
function _pmperson_migrate_node_get_body($nid) {
  return $v = db_query('SELECT body_value FROM {field_data_body} WHERE entity_id = :nid AND entity_type = node',array(':nid' =>$nid))->fetchField();
}

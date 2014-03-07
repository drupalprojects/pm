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
  pmperson_migrate_create_users_if_required();

  // Create and attach fields to Drupal's user account.
  pmperson_migrate_create_fields();

  // Migrate data from pmperson node to Drupal user account.

  // TODO: How to handle cleanup since data is probably required by other
  // modules.
}

/**
 * Checks the database and see if there are any conflicts that would
 * affect the update.
 */
function pmperson_migrate_update_could_be_performed() {
  if (db_table_exists('pmperson')) {
    # code...
  }
  return TRUE;
}
/**
 * Creates Drupal User and associate it with pmperson node.
 */
function pmperson_migrate_create_users_if_required() {
  $unmapped_results = db_select('pmperson', 'pmp')
    ->fields('pmp')
    ->isNull('pmp.user_uid')
    ->execute()
    ->fetchAssoc();
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
  if (!$record OR !$record['nid'] !$account OR !$account->uid) {
    watchdog('pmperson', 'message', array('record' => $record), WATCHDOG_NOTICE);
    return FALSE;
  }
  db_update('pmperson')
    ->fields(array('user_uid' => $account->uid))
    ->condition('nid', $record['nid'])
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
function pmperson_migrate_create_fields() {
  module_load_include('inc', 'pmperson', 'includes/pmperson.field_base');
  module_load_include('inc', 'pmperson', 'includes/pmperson.field_instance');
  module_load_include('inc', 'pm', 'includes/pm.field');

  $field_bases = pmperson_default_field_bases();
  pm_field_bases_create_if_required($field_bases);

  $field_instances = pmperson_default_field_instances();
  pm_field_instances_create_if_required($field_instances);
}

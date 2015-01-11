<?php
/**
 * @file
 * Migration functions for the PM Task module.
 */

define('PMTASK_MIGRATE_MAX_JOB_PER_BATCH', 5);

/**
 * Helper function for migrating between pmissue to drupal fields.
 */
function pmissue_migrate(&$sandbox) {
  pmissue_migrate_create_fields($sandbox);
  // Migrate data from pmissue node to Drupal user account.
  if (empty($sandbox['pmissue_current_vid_of_field_being_migrated'])) {
    $sandbox['pmissue_current_vid_of_field_being_migrated'] = 0;
  }
  if (pmissue_migrate_field_data($sandbox) == FALSE) {
    $sandbox['#finished'] = 0.5;
    return;
  }

  // We don't need pmissue table anymore, so dropping it.
  db_drop_table('pmissue');
  variable_del('node_options_pmissue');

  // PM Permissions default configurations.
  variable_set('pmpermission_field_assigned_reference', 'pm_assigned');
  variable_set('pmpermission_field_pm_reference', 'pm_manager');
  variable_set('pmpermission_field_parent_reference_for_pmissue', 'pmissue_parent');
  variable_set('pmpermission_node_pmissue_enabled', TRUE);

  module_load_include('inc', 'pmpermission', 'includes/pmpermission.migrate');
  pmpermission_migrate_execute('pmissue');

  $sandbox['#finished'] = 1;
  return 'PM Task nodes have been migrated to field_api based fields successfully.';
}

/**
 * Check if migration needs to be performed.
 */
function pmissue_migrate_update_could_be_performed() {
  return FALSE;
}

/**
 * Creates and attaches fields to Drupal user.
 */
function pmissue_migrate_create_fields(&$sandbox) {
  module_load_include('inc', 'pmissue', 'includes/pmissue.field_base');
  module_load_include('inc', 'pmissue', 'includes/pmissue.field_instance');
  module_load_include('inc', 'pm', 'includes/pm.field');

  $field_bases = pmissue_default_field_bases();
  pm_field_bases_create_if_required($field_bases);

  $field_instances = pmissue_default_field_instances();
  pm_field_instances_create_if_required($field_instances);
  return TRUE;
}

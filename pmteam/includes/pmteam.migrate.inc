<?php
/**
 * @file
 * pmteam.migrate.inc
 */


define('PMTEAM_MIGRATE_MAX_JOB_PER_BATCH', 2);


/**
 * Helper function for migrating between PMTeam to drupal fields.
 */
function pmteam_migrate(&$sandbox) {
  pmteam_migrate_create_fields($sandbox);

}


/**
 * Creates and attaches fields to Drupal user.
 */
function pmteam_migrate_create_fields(&$sandbox) {
  module_load_include('inc', 'pmteam', 'includes/pmteam.field_base');
  module_load_include('inc', 'pmteam', 'includes/pmteam.field_instance');
  module_load_include('inc', 'pm', 'includes/pm.field');

  $field_bases = pmteam_default_field_bases();
  pm_field_bases_create_if_required($field_bases);

  $field_instances = pmteam_default_field_instances();
  pm_field_instances_create_if_required($field_instances);
  return TRUE;
}
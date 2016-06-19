<?php
/**
 * @file
 * Field group definitions.
 */

 /**
 * Implements hook_field_group_info().
 */
function pmtimetracking_field_group_info() {
  $export = array();

  // References
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_references|node|pmtimetracking|default';
  $field_group->group_name = 'group_references';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'References',
    'weight' => '0',
    'children' => array(
      0 => 'pmtimetracking_parent',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-references field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_references|node|pmtimetracking|form';
  $field_group->group_name = 'group_references';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'References',
    'weight' => '0',
    'children' => array(
      0 => 'pmtimetracking_parent',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-references field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // Time
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_time|node|pmtimetracking|default';
  $field_group->group_name = 'group_time';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Time',
    'weight' => '0',
    'children' => array(
      0 => 'pm_date',
      1 => 'pm_duration',
      2 => 'pm_durationunit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-time field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_time|node|pmtimetracking|form';
  $field_group->group_name = 'group_time';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Time',
    'weight' => '0',
    'children' => array(
      0 => 'pm_date',
      1 => 'pm_duration',
      2 => 'pm_durationunit',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-time field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // Billing
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_billing|node|pmtimetracking|default';
  $field_group->group_name = 'group_billing';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Billing',
    'weight' => '0',
    'children' => array(
      0 => 'pm_billing_status',
      1 => 'pm_billing_duration',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-billing field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_billing|node|pmtimetracking|form';
  $field_group->group_name = 'group_billing';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Billing',
    'weight' => '0',
    'children' => array(
      0 => 'pm_billing_status',
      1 => 'pm_billing_duration',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-billing field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // Miscellaneous
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_miscellaneous|node|pmtimetracking|default';
  $field_group->group_name = 'group_miscellaneous';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Miscellaneous',
    'weight' => '1',
    'children' => array(
      0 => 'body',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-miscellaneous field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_miscellaneous|node|pmtimetracking|form';
  $field_group->group_name = 'group_miscellaneous';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmtimetracking';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Miscellaneous',
    'weight' => '1',
    'children' => array(
      0 => 'body',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-miscellaneous field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  return $export;
}

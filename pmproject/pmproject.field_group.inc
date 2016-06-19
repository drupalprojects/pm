<?php
/**
 * @file
 * Field group definitions.
 */

 /**
 * Implements hook_field_group_info().
 */
function pmproject_field_group_info() {
  $export = array();

  // References
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_references|node|pmproject|default';
  $field_group->group_name = 'group_references';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'References',
    'weight' => '0',
    'children' => array(
      0 => 'pmproject_parent',
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
  $field_group->identifier = 'group_references|node|pmproject|form';
  $field_group->group_name = 'group_references';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'References',
    'weight' => '0',
    'children' => array(
      0 => 'pmproject_parent',
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

  // Workflow
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_workflow|node|pmproject|default';
  $field_group->group_name = 'group_workflow';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Workflow',
    'weight' => '1',
    'children' => array(
      0 => 'pm_projectstatus',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-workflow field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_workflow|node|pmproject|form';
  $field_group->group_name = 'group_workflow';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Workflow',
    'weight' => '1',
    'children' => array(
      0 => 'pm_projectstatus',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-workflow field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // People
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_people|node|pmproject|default';
  $field_group->group_name = 'group_people';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'People',
    'weight' => '2',
    'children' => array(
      0 => 'pm_manager',
      1 => 'pm_assigned',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-people field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_people|node|pmproject|form';
  $field_group->group_name = 'group_people';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'People',
    'weight' => '2',
    'children' => array(
      0 => 'pm_manager',
      1 => 'pm_assigned',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-people field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // Classification
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_classification|node|pmproject|default';
  $field_group->group_name = 'group_classification';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Classification',
    'weight' => '3',
    'children' => array(
      0 => 'pm_projectpriority',
      1 => 'pm_projectcategory',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-classification field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_classification|node|pmproject|form';
  $field_group->group_name = 'group_classification';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Classification',
    'weight' => '3',
    'children' => array(
      0 => 'pm_projectpriority',
      1 => 'pm_projectcategory',
    ),
    'format_type' => 'fieldset',
    'format_settings' => array(
      'formatter' => 'collapsible',
      'instance_settings' => array(
        'description' => '',
        'classes' => 'group-classification field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // Timing
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_timing|node|pmproject|default';
  $field_group->group_name = 'group_timing';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Timing',
    'weight' => '4',
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
        'classes' => 'group-timing field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_timing|node|pmproject|form';
  $field_group->group_name = 'group_timing';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Timing',
    'weight' => '4',
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
        'classes' => 'group-timing field-group-fieldset',
        'required_fields' => 1,
      ),
    ),
  );
  $export[$field_group->identifier] = $field_group;

  // Billing
  $field_group = new stdClass();
  $field_group->disabled = FALSE; /* Edit this to true to make a default field_group disabled initially */
  $field_group->api_version = 1;
  $field_group->identifier = 'group_billing|node|pmproject|default';
  $field_group->group_name = 'group_billing';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Billing',
    'weight' => '5',
    'children' => array(
      0 => 'pm_currency',
      1 => 'pm_price',
      2 => 'pm_pricemode',
      3 => 'pm_billing_status',
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
  $field_group->identifier = 'group_billing|node|pmproject|form';
  $field_group->group_name = 'group_billing';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Billing',
    'weight' => '5',
    'children' => array(
      0 => 'pm_currency',
      1 => 'pm_price',
      2 => 'pm_pricemode',
      3 => 'pm_billing_status',
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
  $field_group->identifier = 'group_miscellaneous|node|pmproject|default';
  $field_group->group_name = 'group_miscellaneous';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'default';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Miscellaneous',
    'weight' => '6',
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
  $field_group->identifier = 'group_miscellaneous|node|pmproject|form';
  $field_group->group_name = 'group_miscellaneous';
  $field_group->entity_type = 'node';
  $field_group->bundle = 'pmproject';
  $field_group->mode = 'form';
  $field_group->parent_name = '';
  $field_group->data = array(
    'label' => 'Miscellaneous',
    'weight' => '6',
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

<?php

/**
 * @file
 * Dashboard Menu Admin settings.
 */

/**
 * Get list of menu types.
 */
function pm_dashboard_menu_list() {
  $header = array(
    'title' => t('Menu Type'),
    'icon' => t('Description'),
    'main_link' => t('Operations'),
  );

  $rows = array();

  $rows[] = array(
    t('Page'),
    t('Drupal PM Dashboard Menu shown on Page'),
    l('Manage', 'admin/config/pm/pm/dashboard-menu/page'),
  );

  $rows[] = array(
    t('Block'),
    t('Drupal PM Dashboard Menu shown in Block'),
    l('Manage', 'admin/config/pm/pm/dashboard-menu/block'),
  );

  return array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows
  );
}


/**
 * Generates a table of recommended modules.
 */
function pm_dashboard_menu_admin($form, $form_state, $type) {
  drupal_set_title('Dashboard Menu : ' . $type);
  $menu_items = pm_dashboard_menu_get_links($type);

  $form['type'] = array(
    '#type' => 'value',
    '#value' => $type,
  );

  $form['items'] = array(
    '#type' => 'fieldset',
    '#title' => t('Menu items'),
    '#tree' => TRUE,
    '#theme' => 'pm_dashboard_menu_entry_table'
  );

  $form['actions']['add'] = array(
    '#type' => 'submit',
    '#value' => t('Add New item'),
    '#name' => 'add_new_item',
    '#submit' => array('pm_dashboard_menu_admin_add_new'),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  if (isset($form_state['values']['add_new_item'])) {
    $menu_items[] = array(
      'title' => '',
      'path' => '',
      'icon' => '',
      'status' => TRUE,
      'delete' => FALSE,
      'weight' => 0,
      'extra_link' => '',
      'custom' => TRUE,
    );
  }
  $table = &$form['items'];
  $icon_picker = module_exists('fontawesome_iconpicker') ? 'fontawesome_iconpicker_textfield' : 'textfield';
  // Loop through the activites and add them to the form.
  if (!empty($menu_items)) {
    uasort($menu_items, 'drupal_sort_weight');
    foreach ($menu_items as $key => $item) {
      $table[$key]['#menu_item'] = (array)$item;
      $table[$key]['custom'] = array(
        '#type' => 'value',
        '#value' => $item['custom'],
      );
      $table[$key]['id'] = array(
        '#type' => 'value',
        '#value' => $item['id'],
      );
      $table[$key]['title'] = array(
        '#type' => 'textfield',
        '#size' => '30',
        '#default_value' => check_plain($item['title']),
        '#disabled' => !$item['custom'],
        '#attributes' => array(
          'placeholder' => t('Title'),
        ),
      );
      $table[$key]['path'] = array(
        '#type' => 'textfield',
        '#size' => '40',
        '#default_value' => $item['path'],
        '#disabled' => !$item['custom'],
        '#attributes' => array(
          'placeholder' => t('Main Link'),
        ),
      );
      $table[$key]['extra_link'] = array(
        '#type' => 'textfield',
        '#size' => '40',
        '#default_value' => $item['extra_link'],
        '#access' => $item['custom'],
        '#attributes' => array(
          'placeholder' => t('Sub link (add item)'),
        ),
      );
      $table[$key]['icon'] = array(
        '#type' => $icon_picker,
        '#size' => '15',
        '#default_value' => $item['icon'],
        '#disabled' => !$item['custom'],
      );
      // The weight (this is for the tabledrag we'll add in the theme function
      $table[$key]['weight'] = array(
        '#type' => 'textfield',
        '#size' => 2,
        '#delta' => 10,
        '#default_value' => $item['weight'],
      );
      // Is this activity enabled?
      $table[$key]['status'] = array(
        '#type' => 'checkbox',
        '#default_value' => $item['status'] ? TRUE : FALSE,
      );
      // Is this activity enabled?
      $table[$key]['delete'] = array(
        '#type' => 'checkbox',
        '#default_value' => FALSE,
        '#access' => $item['custom'],
      );
    }
  }
  return $form;
}

/**
 * Add new item form callback.
 */
function pm_dashboard_menu_admin_add_new($form, &$form_state) {
  if (isset($form_state['values']['add_new_item'])) {
    $form_state['rebuild'] = TRUE;
    return;
  }
  $form_state['redirect'] = current_path();
}

/**
 * Submit callback for dashboard_menu_admin form.
 */
function pm_dashboard_menu_admin_submit($form, $form_state) {
  $type = $form_state['values']['type'];
  $links = $form_state['values']['items'];
  dpm($links);
  pm_dashboard_menu_save_links($type, $links);
}

/**
 * Get all menu defined for particular type.
 */
function pm_dashboard_menu_get_links($type) {
  $links = array();
  $menu_items = module_invoke_all('pm_dashboard_links', $type);
  if (!empty($menu_items)) {
    foreach ($menu_items as $item) {
      $id = $item['path'];
      $links[$id] = array(
        'custom' => FALSE,
        'title' => $item['title'],
        'icon' => $item['icon'],
        'path' => $item['path'],
        'weight' => $item['weight'],
        'extra_link' => ($item['add_type']) ? 'node/add/' . $item['add_type'] : '',
        'status' => TRUE,
        'id' => $id,
      );
    }
  }
  $menu_items = variable_get('pm_dashboard_links_' . $type, array());
  if (!empty($menu_items)) {
    foreach ($menu_items as $id => $item) {
      if ($item['custom'] == FALSE) {
        $links[$id]['status'] = $item['status'];
        $links[$id]['weight'] = $item['weight'];
      }
      else {
        $links[$id] = $item;
      }
    }
  }
  return $links;
}

/**
 * Save link specific settings for dashboard menu item.
 */
function pm_dashboard_menu_save_links($type, $items) {
  $menu_items = variable_get('pm_dashboard_links_' . $type, array());
  foreach ($items as $item) {
    if (!empty($item['delete'])) {
      unset($menu_items[$item['id']]);
      drupal_set_message(t('Deleting menu with id %id', array('%id' => $item['id'])), 'warning');
      continue;
    }

    if ($item['custom'] == FALSE) {
      $item['id'] = $item['path'];
    }

    if (empty($item['id'])) {
      $item['id'] = uniqid('pm_dashboard_menu_custom_');
      $menu_items[$item['id']] = $item;
    }
    else {
      $menu_items[$item['id']] = $item;
    }
  }
  variable_set('pm_dashboard_links_' . $type,  $menu_items);
}

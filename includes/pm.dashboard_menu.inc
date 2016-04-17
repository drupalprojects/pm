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


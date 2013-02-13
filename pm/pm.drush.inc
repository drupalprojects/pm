<?php

function pm_drush_command() {
  $items['pm-reinstall'] = array(
    'description' => dt('Disable, Uninstall, Reinstall pm modules in the correct order, in one cmd.'),
    'examples' => array(
      'drush pm-reinstall' => 'Disable, Unistall, and Reinstall Project Management & sub modules'
    ),
  'aliases' => array('stre'),
  );
  return $items;
}

function drush_pm_reinstall() {
  $sub_mods = array(
    'pmexpense',
    'pminvoice',
    'pmnote',
    'pmorganization',
    'pmperson',
    'pmproject',
    'pmtask',
    'pmteam',
    'pmticket',
    'pmtimetracking');

  drush_invoke_process(@self, 'pm-disable', array('pm'));

  drush_invoke_process(@self, 'pm-uninstall', array('pmexpense', 'pminvoice', 'pmnote', 'pmteam', 'pmtimetracking'));

  drush_invoke_process(@self, 'pm-uninstall', array('pmperson', 'pmticket'));

  drush_invoke_process(@self, 'pm-uninstall', array('pmtask'));

  drush_invoke_process(@self, 'pm-uninstall', array('pmproject'));

  drush_invoke_process(@self, 'pm-uninstall', array('pmorganization'));

  drush_invoke('pm-enable', $sub_mods);
}


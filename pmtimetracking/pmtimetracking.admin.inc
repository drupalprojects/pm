<?php

/**
 * @file
 */

function pmtimetracking_list() {
  $breadcrumb = array();
  $breadcrumb[] = l(t('Project Management'), 'pm');
  drupal_set_breadcrumb($breadcrumb);

  if (array_key_exists('organization_nid', $_GET)) {
    if ($_SESSION['pmtimetracking_list_filter']['organization_nid'] != $_GET['organization_nid']) {
      $_SESSION['pmtimetracking_list_filter']['organization_nid'] = $_GET['organization_nid'];
    }
    unset($_SESSION['pmtimetracking_list_filter']['project_nid']);
    unset($_SESSION['pmtimetracking_list_filter']['task_nid']);
    unset($_SESSION['pmtimetracking_list_filter']['ticket_nid']);
  }

  if (array_key_exists('project_nid', $_GET)) {
    if ($_SESSION['pmtimetracking_list_filter']['project_nid'] != $_GET['project_nid']) {
      $_SESSION['pmtimetracking_list_filter']['project_nid'] = $_GET['project_nid'];
    }
    $p = node_load($_GET['project_nid']);
    $_SESSION['pmtimetracking_list_filter']['organization_nid'] = $p->organization_nid;

    unset($_SESSION['pmtimetracking_list_filter']['task_nid']);
    unset($_SESSION['pmtimetracking_list_filter']['ticket_nid']);
  }

  if (array_key_exists('task_nid', $_GET)) {
    if ($_SESSION['pmtimetracking_list_filter']['task_nid'] != $_GET['task_nid']) {
      $_SESSION['pmtimetracking_list_filter']['task_nid'] = $_GET['task_nid'];
    }

    $t = node_load($_GET['task_nid']);
    $_SESSION['pmtimetracking_list_filter']['organization_nid'] = $t->organization_nid;
    $_SESSION['pmtimetracking_list_filter']['project_nid'] = $t->project_nid;
    unset($_SESSION['pmtimetracking_list_filter']['ticket_nid']);
  }

  if (array_key_exists('ticket_nid', $_GET)) {
    if ($_SESSION['pmtimetracking_list_filter']['ticket_nid'] != $_GET['ticket_nid']) {
      $_SESSION['pmtimetracking_list_filter']['ticket_nid'] = $_GET['ticket_nid'];
    }

    $ti = node_load($_GET['ticket_nid']);
    $_SESSION['pmtimetracking_list_filter']['organization_nid'] = $ti->organization_nid;
    $_SESSION['pmtimetracking_list_filter']['project_nid'] = $ti->project_nid;
    $_SESSION['pmtimetracking_list_filter']['task_nid'] = $ti->task_nid;
  }

  $i = new stdClass();
  $i->type = 'pmtimetracking';

  $report_attrs = array(
    'onclick' => "pm_popup(this, 'timetracking_list_form_report', 'Timetrackings', 300, 150, 'pmtimetracking_list_form_report', 'l'); return false;",
  );

  $header = array(
    array(
      'data' => t('Organization'),
      'field' => 'stt.organization_title',
    ),
    array(
      'data' => t('Project'),
      'field' => 'stt.project_title',
    ),
    array(
      'data' => t('Title'),
      'field' => 'n.title',
    ),
    array(
      'data' => t('Date'),
      'field' => 'stt.trackingdate',
      'sort' => 'desc',
    ),
    array(
      'data' => t('Duration (h)'),
      'style' => 'text-align: right;'
    ),
    array(
      'data' => pm_icon_l('report', '', t('Reports'), '', array(), $report_attrs) .'&nbsp;'. pm_icon_add_node($i, $_GET),
      'class' => 'pm_list_operations',
    ),
  );

  $s = "SELECT n.*,  stt.* FROM {node} n INNER JOIN {pmtimetracking} stt ON n.vid=stt.vid
  WHERE n.status=1 AND n.type='pmtimetracking' ";

  $s_duration = "SELECT SUM(billing_duration) FROM {pmtimetracking} stt INNER JOIN {node} n ON n.vid=stt.vid WHERE n.status=1 AND n.type='pmtimetracking' ";

  $where = array();
  $args = array();
  $filterfields = array();

  if (isset($_SESSION['pmtimetracking_list_filter']['organization_nid']) && $_SESSION['pmtimetracking_list_filter']['organization_nid'] != 0) {
    $where[] = 'stt.organization_nid=%d';
    $args[] = $_SESSION['pmtimetracking_list_filter']['organization_nid'];
    $filterfields[] = t('Organization');
  }
  if (isset($_SESSION['pmtimetracking_list_filter']['project_nid']) && $_SESSION['pmtimetracking_list_filter']['project_nid'] != 0) {
    $where[] = 'stt.project_nid=%d';
    $args[] = $_SESSION['pmtimetracking_list_filter']['project_nid'];
    $filterfields[] = t('Project');
  }
  if (isset($_SESSION['pmtimetracking_list_filter']['task_nid']) && $_SESSION['pmtimetracking_list_filter']['task_nid'] != 0) {
    $where[] = 'stt.task_nid=%d';
    $args[] = $_SESSION['pmtimetracking_list_filter']['task_nid'];
    $filterfields[] = t('Task');
  }
  if (isset($_SESSION['pmtimetracking_list_filter']['ticket_nid']) && $_SESSION['pmtimetracking_list_filter']['ticket_nid'] != 0) {
    $where[] = 'stt.ticket_nid=%d';
    $args[] = $_SESSION['pmtimetracking_list_filter']['ticket_nid'];
    $filterfields[] = t('Ticket');
  }
  if (isset($_SESSION['pmtimetracking_list_filter']['datefrom']) && $_SESSION['pmtimetracking_list_filter']['datefrom']['day'] != -1) {
    $datefrom = $_SESSION['pmtimetracking_list_filter']['datefrom'];
    $datefrom['hour'] = 0;
    $datefrom['minute'] = 0;
    $where[] = 'stt.trackingdate>=%d';
    $args[] = _pm_datetime_to_gmtimestamp($datefrom);
    $filterfields[] = t('Date');
  }
  if (isset($_SESSION['pmtimetracking_list_filter']['dateto']) && $_SESSION['pmtimetracking_list_filter']['dateto']['day'] != -1) {
    $dateto = $_SESSION['pmtimetracking_list_filter']['dateto'];
    $dateto['hour'] = 23;
    $dateto['minute'] = 59;
    $where[] = 'stt.trackingdate<=%d';
    $args[] = _pm_datetime_to_gmtimestamp($dateto);
    $filterfields[] = t('Date');
  }
  if (isset($_SESSION['pmtimetracking_list_filter']['billable']) && $_SESSION['pmtimetracking_list_filter']['billable'] != '-') {
    $where[] = 'stt.billable=%d';
    $args[] = $_SESSION['pmtimetracking_list_filter']['billable'];
    $filterfields[] = t('Billable');
  }
  if (isset($_SESSION['pmtimetracking_list_filter']['billed']) && $_SESSION['pmtimetracking_list_filter']['billed'] != '-') {
    $where[] = 'stt.billed=%d';
    $args[] = $_SESSION['pmtimetracking_list_filter']['billed'];
    $filterfields[] = t('Billed');
  }

  $itemsperpage = $_SESSION['pmtimetracking_list_filter']['itemsperpage'];
  if (!isset($itemsperpage)) {
    $itemsperpage = variable_get('pm_default_items_per_page', 10);
    $_SESSION['pmtimetracking_list_filter']['itemsperpage'] =  $itemsperpage;
  }

  if (isset($_SESSION['pmtimetracking_list_filter']['user']) && $_SESSION['pmtimetracking_list_filter']['user'] != '') {
    $trackinguser = user_load(array('name' => $_SESSION['pmtimetracking_list_filter']['user']));
    if ($trackinguser === FALSE) $trackinguser->uid = -1;
    $where[] = 'n.uid=%d';
    $args[] = $trackinguser->uid;
    $filterfields[] = t('User');
  }

  if (count($filterfields) == 0) {
    $filterdesc = t('Not filtered');
  }
  else {
    $filterdesc = t('Filtered by !fields', array('!fields' => implode(", ", array_unique($filterfields))));
  }
  $filterdesc .= ' | '. t('!items items per page', array('!items' => $_SESSION['pmtimetracking_list_filter']['itemsperpage']));

  $o = drupal_get_form('pmtimetracking_list_filter', $filterdesc);

  $s = pmtimetracking_access_sql($s, $where);
  $s = db_rewrite_sql($s);

  $tablesort = tablesort_sql($header);
  $r = pager_query($s . $tablesort, $itemsperpage, 0, NULL, $args);

  $s_duration = pmtimetracking_access_sql($s_duration, $where);
  $s_duration = db_rewrite_sql($s_duration);
  $duration = db_result(db_query($s_duration, $args));

  $_SESSION['pmtimetracking_list_filter']['sql'] = $s . $tablesort;
  $_SESSION['pmtimetracking_list_filter']['args'] = $args;

  $timetrackings = array();
  while ($item = db_fetch_object($r)) {
    $timetrackings[] = $item;
  }

  $o .= theme('pmtimetracking_list', $header, $timetrackings, $duration);
  $o .= theme('pager', NULL, $itemsperpage, 0);
  $o .= drupal_get_form('pmtimetracking_list_form_report');
  print theme('page', $o);
}

function pmtimetracking_list_filter(&$form_state, $filterdesc = 'Filter') {
  $organization_nid = $_SESSION['pmtimetracking_list_filter']['organization_nid'];
  $project_nid = $_SESSION['pmtimetracking_list_filter']['project_nid'];
  $task_nid = $_SESSION['pmtimetracking_list_filter']['task_nid'];
  $ticket_nid = $_SESSION['pmtimetracking_list_filter']['ticket_nid'];
  $itemsperpage = $_SESSION['pmtimetracking_list_filter']['itemsperpage'];
  if (!isset($itemsperpage)) {
    $itemsperpage = variable_get('pm_default_items_per_page', 10);
    $_SESSION['pmtimetracking_list_filter']['itemsperpage'] = $itemsperpage;
  }

  $datefrom = isset($_SESSION['pmtimetracking_list_filter']['datefrom']) ? $_SESSION['pmtimetracking_list_filter']['datefrom'] : NULL;
  $dateto = isset($_SESSION['pmtimetracking_list_filter']['dateto']) ? $_SESSION['pmtimetracking_list_filter']['dateto'] : NULL ;

  $trackinguser = $_SESSION['pmtimetracking_list_filter']['user'];

  $form = array();

  $form['filter'] = array(
        '#type' => 'fieldset',
        '#title' => $filterdesc,
        '#collapsible' => TRUE,
        '#collapsed' => TRUE,
        '#theme' => 'pmtimetracking_list_filter',
  );

  $form['filter']['group1'] = array(
    '#type' => 'markup',
    '#theme' => 'pm_form_group',
    '#weight' => -20,
  );

  $s = "SELECT n.nid, n.title FROM {node} AS n INNER JOIN {pmorganization} AS sor ON sor.vid=n.vid
  WHERE n.status=1 AND n.type='pmorganization' AND isactive=1 ORDER BY n.title";
  $s = pmorganization_access_sql($s);
  $s = db_rewrite_sql($s);
  $r = db_query($s);
  $organizations = array();
  while ($organization = db_fetch_object($r)) {
    $organizations[$organization->nid] = $organization->title;
  }
  $organizations = array(0 => t('All')) + $organizations;
  $form['filter']['group1']['organization_nid'] = array(
    '#type' => 'select',
    '#title' => t('Organization'),
    '#default_value' => $organization_nid,
    '#options' => $organizations,
    '#attributes' => array('onchange' => "pmticket_organization_project_task_tickets(this, 'edit-project-nid', 'edit-task-nid', 'edit-ticket-nid', true, 'All')"),
  );

  $s = "SELECT n.nid, n.title FROM {node} AS n INNER JOIN {pmproject} AS spr ON spr.vid=n.vid
  WHERE n.status=1 AND spr.organization_nid=%d AND n.type='pmproject' ORDER BY n.title";
  $s = pmproject_access_sql($s);
  $s = db_rewrite_sql($s);
  $r = db_query($s, $organization_nid);
  $projects = array();
  while ($project = db_fetch_object($r)) {
    $projects[$project->nid] = $project->title;
  }
  $projects = array(0 => t('All')) + $projects;
  $form['filter']['group1']['project_nid'] = array(
    '#type' => 'select',
    '#title' => t('Project'),
    '#default_value' => $project_nid,
    '#options' => $projects,
    '#process' => array('pm_dependent_select_process'),
    '#attributes' => array('onchange' => "pmticket_project_task_tickets(this, 'edit-organization-nid', 'edit-task-nid', 'edit-ticket-nid', true, 'All')"),
  );

  $tree = _pmtask_get_tree($project_nid);
  $tasks = _pmtask_plain_tree($tree);
  $tasks = array(0 => t('All')) + $tasks;
  $form['filter']['group1']['task_nid'] = array(
    '#type' => 'select',
    '#title' => t('Task'),
    '#default_value' => $task_nid,
    '#options' => $tasks,
    '#process' => array('pm_dependent_select_process'),
    '#attributes' => array('onchange' => "pmticket_task_tickets(this, 'edit-organization-nid', 'edit-project-nid', 'edit-ticket-nid', true, 'All')"),
  );

  $s = "SELECT n.nid, n.title FROM {node} AS n INNER JOIN {pmticket} AS sti
  ON sti.vid=n.vid WHERE n.status=1 AND n.type='pmticket' AND sti.organization_nid=%d AND sti.project_nid=%d AND sti.task_nid=%d ORDER BY title ";
  $s = pmticket_access_sql($s);
  $s = db_rewrite_sql($s);
  $r = db_query($s, $organization_nid, $project_nid, $task_nid);
  $tickets = array();
  while ($ticket = db_fetch_object($r)) {
    $tickets[$ticket->nid] = $ticket->title;
  }
  $tickets = array(0 => t('All')) + $tickets;
  $form['filter']['group1']['ticket_nid'] = array(
    '#type' => 'select',
    '#title' => t('Ticket'),
    '#default_value' => $ticket_nid,
    '#process' => array('pm_dependent_select_process'),
    '#options' => $tickets,
  );

  $form['filter']['group2'] = array(
    '#type' => 'markup',
    '#theme' => 'pm_form_group',
    '#weight' => -19,
  );

  $form['filter']['group2']['datefrom'] = array(
    '#type' => 'dateext',
    '#title' => t('Date from'),
    '#withnull' => TRUE,
    '#default_value' => $datefrom,
  );

  $form['filter']['group2']['dateto'] = array(
    '#type' => 'dateext',
    '#title' => t('Date to'),
    '#withnull' => TRUE,
    '#default_value' => $dateto,
  );

  $form['filter']['group2']['user'] = array(
    '#type' => 'textfield',
    '#title' => t('User'),
    '#size' => 25,
    '#default_value' => $trackinguser,
    '#autocomplete_path' => 'user/autocomplete',
  );

  $form['filter']['group3'] = array(
    '#type' => 'markup',
    '#theme' => 'pm_form_group',
  );

  $form['filter']['group3']['billable'] = array(
    '#type' => 'select',
    '#title' => 'Billable',
    '#options' => array('-' => t('all'), '1' => t('billable'), '0' => t('not billable')),
    '#default_value' => $_SESSION['pmtimetracking_list_filter']['billable'],
  );

  $form['filter']['group3']['billed'] = array(
    '#type' => 'select',
    '#title' => 'Billed',
    '#options' => array('-' => t('all'), '1' => t('billed'), '0' => t('not billed')),
    '#default_value' => $_SESSION['pmtimetracking_list_filter']['billed'],
  );

  $form['filter']['group4'] = array(
    '#type' => 'markup',
    '#theme' => 'pm_form_group',
  );

  $form['filter']['group4']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
    '#submit' => array('pmtimetracking_list_filter_filter'),
  );

  $form['filter']['group4']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('pmtimetracking_list_filter_reset'),
  );

  $form['filter']['group4']['itemsperpage'] = array(
    '#type' => 'textfield',
    '#title' => t('Items'),
    '#size' => 10,
    '#default_value' => $itemsperpage,
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );

  return $form;
}

function pmtimetracking_list_filter_filter($form, &$form_state) {
  $_SESSION['pmtimetracking_list_filter']['organization_nid'] = $form_state['values']['organization_nid'];
  $_SESSION['pmtimetracking_list_filter']['project_nid'] = $form_state['values']['project_nid'];
  $_SESSION['pmtimetracking_list_filter']['task_nid'] = $form_state['values']['task_nid'];
  $_SESSION['pmtimetracking_list_filter']['ticket_nid'] = $form_state['values']['ticket_nid'];
  $_SESSION['pmtimetracking_list_filter']['datefrom'] = $form_state['values']['datefrom'];
  $_SESSION['pmtimetracking_list_filter']['dateto'] = $form_state['values']['dateto'];
  $_SESSION['pmtimetracking_list_filter']['billable'] = $form_state['values']['billable'];
  $_SESSION['pmtimetracking_list_filter']['billed'] = $form_state['values']['billed'];
  $_SESSION['pmtimetracking_list_filter']['itemsperpage'] = $form_state['values']['itemsperpage'];
  $_SESSION['pmtimetracking_list_filter']['user'] = $form_state['values']['user'];
}

function pmtimetracking_list_filter_reset($form, &$form_state) {
  unset($_SESSION['pmtimetracking_list_filter']);
}

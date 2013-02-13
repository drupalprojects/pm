<?php

/**
 * @file
 */

function pmticket_list() {
  global $user;

  $breadcrumb = array();
  $breadcrumb[] = l(t('Project Management'), 'pm');
  drupal_set_breadcrumb($breadcrumb);

  if (array_key_exists('organization_nid', $_GET)) {
    if ($_SESSION['pmticket_list_filter']['organization_nid'] != $_GET['organization_nid']) {
      $_SESSION['pmticket_list_filter']['organization_nid'] = $_GET['organization_nid'];
    }
    unset($_SESSION['pmticket_list_filter']['project_nid']);
    unset($_SESSION['pmticket_list_filter']['task_nid']);
  }

  if (array_key_exists('project_nid', $_GET)) {
    if ($_SESSION['pmticket_list_filter']['project_nid'] != $_GET['project_nid']) {
      $_SESSION['pmticket_list_filter']['project_nid'] = $_GET['project_nid'];
    }
    $p = node_load($_GET['project_nid']);
    $_SESSION['pmticket_list_filter']['organization_nid'] = $p->organization_nid;

    unset($_SESSION['pmticket_list_filter']['task_nid']);
  }

  if (array_key_exists('task_nid', $_GET)) {
    if ($_SESSION['pmticket_list_filter']['task_nid'] != $_GET['task_nid']) {
      $_SESSION['pmticket_list_filter']['task_nid'] = $_GET['task_nid'];
    }

    $t = node_load($_GET['task_nid']);
    $_SESSION['pmticket_list_filter']['organization_nid'] = $t->organization_nid;
    $_SESSION['pmticket_list_filter']['project_nid'] = $t->project_nid;
  }

  $i = new stdClass();
  $i->type = 'pmticket';

  $header = array(
    array(
      'data' => ' ',
    ),
    array(
      'data' => t('Organization'),
      'field' => 'sti.organization_title',
    ),
    array(
      'data' => t('Project'),
      'field' => 'sti.project_title',
    ),
    array(
      'data' => t('Title'),
      'field' => 'n.title',
    ),
    array(
      'data' => t('Date'),
      'field' => 'n.created',
      'sort' => 'desc',
    ),
    array(
      'data' => t('Status'),
      'field' => 'sti.ticketstatus',
    ),
    array(
      'data' => t('Priority'),
      'field' => 'sti.ticketpriority',
    ),
    array(
      'data' => pm_icon_add_node($i, $_GET),
      'class' => 'pm_list_operations',
    ),
  );

  $where = array();
  $args = array();
  $filterfields = array();

  $tic_query = db_select('node', 'n');
  $tic_query->join('pmticket', 'sti', 'n.vid = sti.vid');
  $tic_query
    ->fields('n')
    ->fields('sti')
    ->condition('n.status', 1)
    ->condition('n.type', 'pmticket');

  if (isset($_SESSION['pmticket_list_filter']['organization_nid']) && ($_SESSION['pmticket_list_filter']['organization_nid'] != 0)) {
    $where[] = 'sti.organization_nid=%d';
    $args[] = $_SESSION['pmticket_list_filter']['organization_nid'];
    $filterfields[] = t('Organization');
  }
  if (isset($_SESSION['pmticket_list_filter']['project_nid']) && ($_SESSION['pmticket_list_filter']['project_nid'] != 0)) {
    $where[] = 'sti.project_nid=%d';
    $args[] = $_SESSION['pmticket_list_filter']['project_nid'];
    $filterfields[] = t('Project');
  }
  if (isset($_SESSION['pmticket_list_filter']['task_nid']) && ($_SESSION['pmticket_list_filter']['task_nid'] != 0)) {
    $where[] = 'sti.task_nid=%d';
    $args[] = $_SESSION['pmticket_list_filter']['task_nid'];
    $filterfields[] = t('Task');
  }

  $category_list = pm_attributes_bydomain('ticket category search');
  $ticketcategory = isset($_SESSION['pmticket_list_filter']['ticketcategory']) ? $_SESSION['pmticket_list_filter']['ticketcategory'] : $category_list['default'];
  $_SESSION['pmticket_list_filter']['ticketcategory'] = $ticketcategory;

  if ($ticketcategory != '-') {
    $category = split(',', $ticketcategory);
    $v = array();
    foreach ($category as $item) $v[] = '%s';
    $where[] = "sti.ticketcategory IN ('". implode("','", $v) ."')";
    foreach ($category as $key => $value) {
      $args[] = $value;
    }
    $filterfields[] = t('Category');
  }

  $status_list = pm_attributes_bydomain('ticket status search');
  $ticketstatus = isset($_SESSION['pmticket_list_filter']['ticketstatus']) ? $_SESSION['pmticket_list_filter']['ticketstatus'] : $status_list['default'];
  $_SESSION['pmticket_list_filter']['ticketstatus'] = $ticketstatus;

  if ($ticketstatus != '-') {
    $status = split(',', $ticketstatus);
    $v = array();
    foreach ($status as $item) $v[] = '%s';
    $where[] = "sti.ticketstatus IN ('". implode("','", $v) ."')";
    foreach ($status as $key => $value) {
      $args[] = $value;
    }
    $filterfields[] = t('Status');
  }

  $priority_list = pm_attributes_bydomain('ticket priority search');
  $ticketpriority = isset($_SESSION['pmticket_list_filter']['ticketpriority']) ? $_SESSION['pmticket_list_filter']['ticketpriority'] : $priority_list['default'];
  $_SESSION['pmticket_list_filter']['ticketpriority'] = $ticketpriority;

  if ($ticketpriority != '-') {
    $priority = split(',', $ticketpriority);
    $v = array();
    foreach ($priority as $item) $v[] = '%s';
    $where[] = "sti.ticketpriority IN ('". implode("','", $v) ."')";
    foreach ($priority as $key => $value) {
      $args[] = $value;
    }
    $filterfields[] = t('Priority');
  }

  if (isset($_SESSION['pmticket_list_filter']['datebeginfrom'])) {
    $datebeginfrom = $_SESSION['pmticket_list_filter']['datebeginfrom'];
    $datebeginfrom['hour'] = 0;
    $datebeginfrom['minute'] = 0;
    $t = _pm_datetime_to_gmtimestamp($datebeginfrom);
    if ($datebeginfrom['year']>0 && $t>=0) {
      $where[] = 'sti.datebegin>=%d';
      $args[] = $t;
      $filterfields[] = t('Date');
    }
  }

  if (isset($_SESSION['pmticket_list_filter']['datebeginto'])) {
    $datebeginto = $_SESSION['pmticket_list_filter']['datebeginto'];
    $datebeginto['hour'] = 23;
    $datebeginto['minute'] = 59;
    $t = _pm_datetime_to_gmtimestamp($datebeginto);
    if ($datebeginto['year']>0 && $t>=0) {
      $where[] = 'sti.datebegin<=%d';
      $args[] = $t;
      $filterfields[] = t('Date');
    }
  }

  if (isset($_SESSION['pmticket_list_filter']['dateendfrom'])) {
    $dateendfrom = $_SESSION['pmticket_list_filter']['dateendfrom'];
    $dateendfrom['hour'] = 0;
    $dateendfrom['minute'] = 0;
    $t = _pm_datetime_to_gmtimestamp($dateendfrom);
    if ($dateendfrom['year']>0 && $t>=0) {
      $where[] = 'sti.dateend>=%d';
      $args[] = $t;
      $filterfields[] = t('Date');
    }
  }

  if (isset($_SESSION['pmticket_list_filter']['dateendto'])) {
    $dateendto = $_SESSION['pmticket_list_filter']['dateendto'];
    $dateendto['hour'] = 23;
    $dateendto['minute'] = 59;
    $t = _pm_datetime_to_gmtimestamp($dateendto);
    if ($dateendto['year']>0 && $t>=0) {
      $where[] = 'sti.dateend<=%d';
      $args[] = $t;
      $filterfields[] = t('Date');
    }
  }

  if (isset($_SESSION['pmticket_list_filter']['assigned_to'])) {
    if (!is_numeric($_SESSION['pmticket_list_filter']['assigned_to'])) {
      switch ($_SESSION['pmticket_list_filter']['assigned_to']) {
        case 'all':
          // NO FILTER
          break;
        case 'mine':
          // CURRENT USER
          if (!empty($user->pmperson_nid) && is_numeric($user->pmperson_nid)) {
            $assigned_to_nid = $user->pmperson_nid;
          }
          break;
        case 'none':
          $where[] = '(sti.assigned_nid IS NULL OR sti.assigned_nid = 0) ';
          break;
      }
    }
    else {
      $assigned_to_nid = $_SESSION['pmticket_list_filter']['assigned_to'];
    }
    if (!empty($assigned_to_nid) && is_numeric($assigned_to_nid)) {
      $where[] = 'sti.assigned_nid=%d';
      $args[] = $assigned_to_nid;
      $filterfields[] = t('Assigned to');
    }
  }

  if (isset($_SESSION['pmticket_list_filter']['billable']) && $_SESSION['pmticket_list_filter']['billable'] != '-') {
    $where[] = 'sti.billable=%d';
    $args[] = $_SESSION['pmticket_list_filter']['billable'];
    $filterfields[] = t('Billable');
  }

  if (isset($_SESSION['pmticket_list_filter']['billed']) && $_SESSION['pmticket_list_filter']['billed'] != '-') {
    $where[] = 'sti.billed=%d';
    $args[] = $_SESSION['pmticket_list_filter']['billed'];
    $filterfields[] = t('Billed');
  }

  $itemsperpage = isset($_SESSION['pmticket_list_filter']['itemsperpage']) ? $_SESSION['pmticket_list_filter']['itemsperpage'] : variable_get('pm_default_items_per_page', 10);
  $_SESSION['pmticket_list_filter']['itemsperpage'] = $itemsperpage;

  if (count($filterfields)==0) {
    $filterdesc = t('Not filtered');
  }
  else {
    $filterdesc = t('Filtered by !fields', array('!fields' => implode(", ", array_unique($filterfields))));
  }
  $filterdesc .= ' | '. t('!items items per page', array('!items' => $_SESSION['pmticket_list_filter']['itemsperpage']));

  $pmticket_list_filter = drupal_get_form('pmticket_list_filter', $filterdesc);

  $o = drupal_render($pmticket_list_filter);

//  $s = pmticket_access_sql($s, $where);
//  $s = db_rewrite_sql($s);

//  $tablesort = tablesort_sql($header);
//  $r = pager_query($s . $tablesort, $itemsperpage, 0, NULL, $args);
  $tic_result = $tic_query->execute();

  $tickets = array();
  foreach ($tic_result as $item) {
    $tickets[] = $item;
  }

  $o .= theme('pmticket_list', array('header' => $header, 'tickets' => $tickets));
//  $o .= theme('pager', NULL, $itemsperpage, 0);
  return $o;
}

function pmticket_list_filter($form, &$form_state, $filterdesc='Filter') {
  $organization_nid = isset($_SESSION['pmticket_list_filter']['organization_nid']) ? $_SESSION['pmticket_list_filter']['organization_nid'] : 0;
  $project_nid = isset($_SESSION['pmticket_list_filter']['project_nid']) ? $_SESSION['pmticket_list_filter']['project_nid'] : 0;
  $task_nid = isset($_SESSION['pmticket_list_filter']['task_nid']) ? $_SESSION['pmticket_list_filter']['task_nid'] : 0;

  $category_list = pm_attributes_bydomain('ticket category search');
  $ticketcategory = isset($_SESSION['pmticket_list_filter']['ticketcategory']) ? $_SESSION['pmticket_list_filter']['ticketcategory'] : $category_list['default'];

  $status_list = pm_attributes_bydomain('ticket status search');
  $ticketstatus = isset($_SESSION['pmticket_list_filter']['ticketstatus']) ? $_SESSION['pmticket_list_filter']['ticketstatus'] : $status_list['default'];

  $priority_list = pm_attributes_bydomain('ticket priority search');
  $ticketpriority = isset($_SESSION['pmticket_list_filter']['ticketpriority']) ? $_SESSION['pmticket_list_filter']['ticketpriority'] : $priority_list['default'];

  $itemsperpage = isset($_SESSION['pmticket_list_filter']['itemsperpage']) ? $_SESSION['pmticket_list_filter']['itemsperpage'] : variable_get('pm_default_items_per_page', 10);
  $_SESSION['pmticket_list_filter']['itemsperpage'] = $itemsperpage;

  $datebeginfrom = isset($_SESSION['pmticket_list_filter']['datebeginfrom']) ? $_SESSION['pmticket_list_filter']['datebeginfrom'] : NULL;
  $datebeginto = isset($_SESSION['pmticket_list_filter']['datebeginto']) ? $_SESSION['pmticket_list_filter']['datebeginto'] : NULL;
  $dateendfrom = isset($_SESSION['pmticket_list_filter']['dateendfrom']) ? $_SESSION['pmticket_list_filter']['dateendfrom'] : NULL;
  $dateendto = isset($_SESSION['pmticket_list_filter']['dateendto']) ? $_SESSION['pmticket_list_filter']['dateendto'] : NULL;

  $assigned_to = isset($_SESSION['pmticket_list_filter']['assigned_to']) ? $_SESSION['pmticket_list_filter']['assigned_to'] : NULL;

  $form = array();

  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => $filterdesc,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
//    '#theme' => 'pmticket_list_filter',
  );

  $form['filter']['group1'] = array(
    '#type' => 'markup',
//    '#theme' => 'pm_form_group',
  );

  $org_query = db_select('node', 'n');
  $org_query->join('pmorganization', 'sor', 'n.vid = sor.vid');
  $org_result = $org_query
    ->fields('n', array('nid', 'title'))
    ->condition('n.status', 1)
    ->condition('n.type', 'pmorganization')
    ->orderBy('n.title', 'ASC')
    ->execute();

//  $s = pmorganization_access_sql($s);
//  $s = db_rewrite_sql($s);
  $organizations = array();
  foreach ($org_result as $organization) {
    $organizations[$organization->nid] = $organization->title;
  }
  $organizations = array(0 => t('All')) + $organizations;
  $form['filter']['group1']['organization_nid'] = array(
    '#type' => 'select',
    '#title' => t('Organization'),
    '#default_value' => $organization_nid,
    '#options' => $organizations,
    '#attributes' => array('onchange' => "pmtask_organization_project_tasks(this, 'edit-project-nid', 'edit-task-nid', 'edit-assigned-nid', true, 'All')"),
  );

  $pro_query = db_select('node', 'n');
  $pro_query->join('pmproject', 'spr', 'n.vid = spr.vid');
  $pro_result = $pro_query
    ->fields('n', array('nid', 'title'))
    ->condition('n.status', 1)
    ->condition('spr.organization_nid', $organization_nid)
    ->condition('n.type', 'pmproject')
    ->orderBy('n.title', 'ASC')
    ->execute();

//  $s = pmproject_access_sql($s);
//  $s = db_rewrite_sql($s);
  $projects = array();
  foreach ($pro_result as $project) {
    $projects[$project->nid] = $project->title;
  }
  $projects = array(0 => t('All')) + $projects;
  $form['filter']['group1']['project_nid'] = array(
    '#type' => 'select',
    '#title' => t('Project'),
    '#default_value' => $project_nid,
    '#options' => $projects,
//    '#process' => array('pm_dependent_select_process'),
    '#attributes' => array('onchange' => "pmtask_project_tasks('edit-organization-nid', this, 'edit-task-nid', 'edit-assigned-nid', true, 'All')"),
  );

  $task_query = db_select('node', 'n');
  $task_query->join('pmtask', 'sta', 'n.vid = sta.vid');
  $task_result = $task_query
    ->fields('n', array('nid', 'title'))
    ->condition('n.status', 1)
    ->condition('n.type', 'pmtask')
    ->condition('sta.project_nid', $project_nid)
    ->orderBy('n.title', 'ASC')
    ->execute();

//  $s = pmtask_access_sql($s);
//  $s = db_rewrite_sql($s);
  $tasks = array();
  foreach ($task_result as $task) {
    $tasks[$task->nid] = $task->title;
  }
  $tasks = array(0 => t('All')) + $tasks;
  $form['filter']['group1']['task_nid'] = array(
    '#type' => 'select',
    '#title' => t('Task'),
    '#default_value' => $task_nid,
    '#options' => $tasks,
  );

  $form['filter']['group2'] = array(
    '#type' => 'markup',
//    '#theme' => 'pm_form_group',
  );

  $form['filter']['group2']['ticketcategory'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#default_value' => $ticketcategory,
    '#options' => $category_list['values'],
  );

  $form['filter']['group2']['ticketstatus'] = array(
    '#type' => 'select',
    '#title' => t('Status'),
    '#default_value' => $ticketstatus,
    '#options' => $status_list['values'],
  );

  $form['filter']['group2']['ticketpriority'] = array(
    '#type' => 'select',
    '#title' => t('Priority'),
    '#default_value' => $ticketpriority,
    '#options' => $priority_list['values'],
  );

  $form['filter']['group3'] = array(
    '#type' => 'markup',
//    '#theme' => 'pm_form_group',
  );

  $form['filter']['group3']['datebeginfrom'] = array(
    '#type' => 'dateext',
    '#withnull' => 'true',
    '#title' => t('Date begin from'),
    '#default_value' => $datebeginfrom,
  );

  $form['filter']['group3']['datebeginto'] = array(
    '#type' => 'dateext',
    '#withnull' => 'true',
    '#title' => t('Date begin to'),
    '#default_value' => $datebeginto,
  );

  $form['filter']['group4'] = array(
    '#type' => 'markup',
//    '#theme' => 'pm_form_group',
  );

  $form['filter']['group4']['dateendfrom'] = array(
    '#type' => 'dateext',
    '#withnull' => 'true',
    '#title' => t('Date end from'),
    '#default_value' => $dateendfrom,
  );

  $form['filter']['group4']['dateendto'] = array(
    '#type' => 'dateext',
    '#withnull' => 'true',
    '#title' => t('Date end to'),
    '#default_value' => $dateendto,
  );

  $form['filter']['group4_1'] = array(
    '#type' => 'markup',
//    '#theme' => 'pm_form_group',
  );

  // ASSIGNED TO
//  $options = pm_get_assignment_options(0, 0);
  $form['filter']['group4_1']['assigned_to'] = array(
    '#type' => 'select',
    '#title' => t('Assigned to'),
    '#default_value' => $assigned_to,
    '#options' => array(),// $options,
  );

  $form['filter']['group4_1']['billable'] = array(
    '#type' => 'select',
    '#title' => 'Billable',
    '#options' => array('-' => t('all'), '1' => t('billable'), '0' => t('not billable')),
    '#default_value' => isset($_SESSION['pmticket_list_filter']['billable']) ? $_SESSION['pmticket_list_filter']['billable'] : '-',
  );

  $form['filter']['group4_1']['billed'] = array(
    '#type' => 'select',
    '#title' => 'Billed',
    '#options' => array('-' => t('all'), '1' => t('billed'), '0' => t('not billed')),
    '#default_value' => isset($_SESSION['pmticket_list_filter']['billed']) ? $_SESSION['pmticket_list_filter']['billed'] : '-',
  );

  $form['filter']['group5'] = array(
    '#type' => 'markup',
//    '#theme' => 'pm_form_group',
  );

  $form['filter']['group5']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
    '#submit' => array('pmticket_list_filter_filter'),
  );

  $form['filter']['group5']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('pmticket_list_filter_reset'),
  );

  $form['filter']['group5']['itemsperpage'] = array(
    '#type' => 'textfield',
    '#title' => t('Items'),
    '#size' => 10,
    '#default_value' => $itemsperpage,
    '#prefix' => '<div class="container-inline">',
    '#suffix' => '</div>',
  );

  return $form;
}

function pmticket_list_filter_filter($form, &$form_state) {
  $_SESSION['pmticket_list_filter']['organization_nid'] = $form_state['values']['organization_nid'];
  $_SESSION['pmticket_list_filter']['project_nid'] = $form_state['values']['project_nid'];
  $_SESSION['pmticket_list_filter']['task_nid'] = $form_state['values']['task_nid'];
  $_SESSION['pmticket_list_filter']['ticketcategory'] = $form_state['values']['ticketcategory'];
  $_SESSION['pmticket_list_filter']['ticketstatus'] = $form_state['values']['ticketstatus'];
  $_SESSION['pmticket_list_filter']['ticketpriority'] = $form_state['values']['ticketpriority'];
  $_SESSION['pmticket_list_filter']['datebeginfrom'] = $form_state['values']['datebeginfrom'];
  $_SESSION['pmticket_list_filter']['datebeginto'] = $form_state['values']['datebeginto'];
  $_SESSION['pmticket_list_filter']['dateendfrom'] = $form_state['values']['dateendfrom'];
  $_SESSION['pmticket_list_filter']['dateendto'] = $form_state['values']['dateendto'];
  $_SESSION['pmticket_list_filter']['assigned_to'] = $form_state['values']['assigned_to'];
  $_SESSION['pmticket_list_filter']['billable'] = $form_state['values']['billable'];
  $_SESSION['pmticket_list_filter']['billed'] = $form_state['values']['billed'];
  $_SESSION['pmticket_list_filter']['itemsperpage'] = $form_state['values']['itemsperpage'];
}

function pmticket_list_filter_reset($form, &$form_state) {
  unset($_SESSION['pmticket_list_filter']);
}

function _pmticket_task_tickets_js($organization_nid=0, $project_nid=0, $task_nid=0) {
  $tickets = array();

  $s = "SELECT n.nid, n.title FROM {node} n INNER JOIN {pmticket} sti
  ON n.vid=sti.vid WHERE n.status=1 AND n.type='pmticket' AND sti.organization_nid=%d AND sti.project_nid=%d
  AND sti.task_nid=%d ORDER BY n.title";

  $s = pmticket_access_sql($s);
  $s = db_rewrite_sql($s);
  $r = db_query($s, $organization_nid, $project_nid, $task_nid);
  while ($item = db_fetch_object($r)) {
    $nid = $item->nid;
    $tickets[$nid] = check_plain($item->title);
  }
  print drupal_to_js($tickets);
  exit();
}

<?php

/**
 * @file
 */

function pmexpense_list() {
  $breadcrumb = array();
  $breadcrumb[] = l(t('Project Management'), 'pm');
  drupal_set_breadcrumb($breadcrumb);

  if (array_key_exists('organization_nid', $_GET)) {
    if ($_SESSION['pmexpense_list_filter']['organization_nid'] != $_GET['organization_nid']) {
      $_SESSION['pmexpense_list_filter']['organization_nid'] = $_GET['organization_nid'];
    }
    unset($_SESSION['pmexpense_list_filter']['project_nid']);
    unset($_SESSION['pmexpense_list_filter']['task_nid']);
    unset($_SESSION['pmexpense_list_filter']['ticket_nid']);
  }

  if (array_key_exists('project_nid', $_GET)) {
    if ($_SESSION['pmexpense_list_filter']['project_nid'] != $_GET['project_nid']) {
      $_SESSION['pmexpense_list_filter']['project_nid'] = $_GET['project_nid'];
    }
    $p = node_load($_GET['project_nid']);
    $_SESSION['pmexpense_list_filter']['organization_nid'] = $p->organization_nid;

    unset($_SESSION['pmexpense_list_filter']['task_nid']);
    unset($_SESSION['pmexpense_list_filter']['ticket_nid']);
  }

  if (array_key_exists('task_nid', $_GET)) {
    if ($_SESSION['pmexpense_list_filter']['task_nid'] != $_GET['task_nid']) {
      $_SESSION['pmexpense_list_filter']['task_nid'] = $_GET['task_nid'];
    }

    $t = node_load($_GET['task_nid']);
    $_SESSION['pmexpense_list_filter']['organization_nid'] = $t->organization_nid;
    $_SESSION['pmexpense_list_filter']['project_nid'] = $t->project_nid;
    unset($_SESSION['pmexpense_list_filter']['ticket_nid']);
  }

  if (array_key_exists('ticket_nid', $_GET)) {
    if ($_SESSION['pmexpense_list_filter']['ticket_nid'] != $_GET['ticket_nid']) {
      $_SESSION['pmexpense_list_filter']['ticket_nid'] = $_GET['ticket_nid'];
    }

    $ti = node_load($_GET['ticket_nid']);
    $_SESSION['pmexpense_list_filter']['organization_nid'] = $ti->organization_nid;
    $_SESSION['pmexpense_list_filter']['project_nid'] = $ti->project_nid;
    $_SESSION['pmexpense_list_filter']['task_nid'] = $ti->task_nid;
  }

  $i = new stdClass();
  $i->type = 'pmexpense';

  $report_attrs = array(
    'onclick' => "pm_popup(this, 'expense_list_form_report', 'Expenses', 300, 150, 'pmexpense_list_form_report', 'l'); return false;",
  );

  $header = array(
    array(
      'data' => t('Organization'),
      'field' => 'sex.organization_title',
    ),
    array(
      'data' => t('Project'),
      'field' => 'sex.project_title',
    ),
    array(
      'data' => t('Title'),
      'field' => 'n.title',
    ),
    array(
      'data' => t('Date'),
      'field' => 'sex.expensedate',
      'sort' => 'desc',
    ),
    array(
      'data' => t('Total'),
      'style' => 'text-align: right;'
    ),
    array(
      'data' => pm_icon_l('report', '', t('Reports'), '', array(), $report_attrs) .'&nbsp;'. pm_icon_add_node($i, $_GET),
      'class' => 'pm_list_operations',
    ),
  );

  $s  = "SELECT n.*,  sex.* FROM {node} AS n INNER JOIN {pmexpense} AS sex ON n.vid=sex.vid
  WHERE n.status=1 AND n.type='pmexpense' ";

  $s_totals = "SELECT SUM(amount) AS t_amount, SUM(tax1) AS t_tax1, SUM(tax2) AS t_tax2, SUM(total) AS t_total FROM
  {node} AS n INNER JOIN {pmexpense} AS sex ON n.vid=sex.vid WHERE n.status=1 AND n.type='pmexpense' ";

  $where = array();
  $args = array();
  $filterfields = array();

  if (isset($_SESSION['pmexpense_list_filter']['organization_nid']) && $_SESSION['pmexpense_list_filter']['organization_nid'] != 0) {
    $where[] = 'sex.organization_nid=%d';
    $args[] = $_SESSION['pmexpense_list_filter']['organization_nid'];
    $filterfields[] = 'Organization';
  }
  if (isset($_SESSION['pmexpense_list_filter']['project_nid']) && $_SESSION['pmexpense_list_filter']['project_nid'] != 0) {
    $where[] = 'sex.project_nid=%d';
    $args[] = $_SESSION['pmexpense_list_filter']['project_nid'];
    $filterfields[] = 'Project';
  }
  if (isset($_SESSION['pmexpense_list_filter']['task_nid']) && $_SESSION['pmexpense_list_filter']['task_nid'] != 0) {
    $where[] = 'sex.task_nid=%d';
    $args[] = $_SESSION['pmexpense_list_filter']['task_nid'];
    $filterfields[] = 'Task';
  }
  if (isset($_SESSION['pmexpense_list_filter']['ticket_nid']) && $_SESSION['pmexpense_list_filter']['ticket_nid'] != 0) {
    $where[] = 'sex.ticket_nid=%d';
    $args[] = $_SESSION['pmexpense_list_filter']['ticket_nid'];
    $filterfields[] = 'Ticket';
  }
  if (isset($_SESSION['pmexpense_list_filter']['datefrom']) && $_SESSION['pmexpense_list_filter']['datefrom']['day'] != -1) {
    $datefrom = $_SESSION['pmexpense_list_filter']['datefrom'];
    $datefrom['hour'] = 0;
    $datefrom['minute'] = 0;
    $where[] = 'sex.expensedate>=%d';
    $args[] = _pm_datetime_to_gmtimestamp($datefrom);
    $filterfields[] = 'Date';
  }
  if (isset($_SESSION['pmexpense_list_filter']['title']) && $_SESSION['pmexpense_list_filter']['title'] != '') {
    $where[] = "LOWER(n.title) LIKE LOWER('%s')";
    $args[] = $_SESSION['pmexpense_list_filter']['title'];
    $filterfields[] = 'Title';
  }
  if (isset($_SESSION['pmexpense_list_filter']['provider_title']) && $_SESSION['pmexpense_list_filter']['provider_title'] != '') {
    $where[] = "LOWER(sex.provider_title) LIKE LOWER('%s')";
    $args[] = $_SESSION['pmexpense_list_filter']['provider_title'];
    $filterfields[] = 'Provider';
  }

  $itemsperpage = isset($_SESSION['pmexpense_list_filter']['itemsperpage']) ? $_SESSION['pmexpense_list_filter']['itemsperpage'] : variable_get('pm_default_items_per_page', 10);

  if (isset($_SESSION['pmexpense_list_filter']['user']) && $_SESSION['pmexpense_list_filter']['user'] != '') {
    $expenseuser = user_load(array('name' => $_SESSION['pmexpense_list_filter']['user']));
    if ($expenseuser === FALSE) $expenseuser->uid = -1;
    $where[] = 'n.uid=%d';
    $args[] = $expenseuser->uid;
    $filterfields[] = 'User';
  }

  if (count($filterfields) == 0) {
    $filterdesc = t('Not filtered');
  }
  else {
    $filterdesc = t('Filtered by !fields', array('!fields' => implode(", ", array_unique($filterfields))));
  }
  $filterdesc .= ' | '. t('!items items per page', array('!items' => $itemsperpage));

  $o = drupal_get_form('pmexpense_list_filter', $filterdesc);

  $s = pmexpense_access_sql($s, $where);
  $s = db_rewrite_sql($s);
  $tablesort = tablesort_sql($header);
  $r = pager_query($s . $tablesort, $itemsperpage, 0, NULL, $args);

  $_SESSION['pmexpense_list_filter']['sql'] = $s . $tablesort;
  $_SESSION['pmexpense_list_filter']['args'] = $args;

  $expenses = array();
  while ($item = db_fetch_object($r)) {
    $expenses[] = $item;
  }

  $s_totals = pmexpense_access_sql($s_totals, $where);
  $s_totals = db_rewrite_sql($s_totals);
  $r = db_query($s_totals, $args);
  $totals = db_fetch_object($r);

  $o .= theme('pmexpense_list', $header, $expenses, $totals);
  $o .= theme('pager', NULL, $itemsperpage, 0);
  $o .= drupal_get_form('pmexpense_list_form_report');
  print theme('page', $o);
}

function pmexpense_list_filter(&$form_state, $filterdesc = 'Filter') {
  $organization_nid = isset($_SESSION['pmexpense_list_filter']['organization_nid']) ? $_SESSION['pmexpense_list_filter']['organization_nid'] : 0;
  $project_nid = isset($_SESSION['pmexpense_list_filter']['project_nid']) ? $_SESSION['pmexpense_list_filter']['project_nid'] : 0;
  $task_nid = isset($_SESSION['pmexpense_list_filter']['task_nid']) ? $_SESSION['pmexpense_list_filter']['task_nid'] : 0;
  $ticket_nid = isset($_SESSION['pmexpense_list_filter']['ticket_nid']) ? $_SESSION['pmexpense_list_filter']['ticket_nid'] : 0;
  $itemsperpage = isset($_SESSION['pmexpense_list_filter']['itemsperpage']) ? $_SESSION['pmexpense_list_filter']['itemsperpage'] : variable_get('pm_default_items_per_page', 10);
  $_SESSION['pmexpense_list_filter']['itemsperpage'] = $itemsperpage;
  $title = isset($_SESSION['pmexpense_list_filter']['title']) ? $_SESSION['pmexpense_list_filter']['title'] : '';
  $provider_title = isset($_SESSION['pmexpense_list_filter']['provider_title']) ? $_SESSION['pmexpense_list_filter']['provider_title'] : '';

  $datefrom = isset($_SESSION['pmexpense_list_filter']['datefrom']) ? $_SESSION['pmexpense_list_filter']['datefrom'] : NULL;

  $expenseuser = isset($_SESSION['pmexpense_list_filter']['user']) ? $_SESSION['pmexpense_list_filter']['user'] : '';

  $form = array();

  $form['filter'] = array(
    '#type' => 'fieldset',
    '#title' => $filterdesc,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#theme' => 'pmexpense_list_filter',
  );

  $form['filter']['group1'] = array(
    '#type' => 'markup',
    '#theme' => 'pm_form_group',
    '#weight' => -20,
  );

  $s = "SELECT n.nid, n.title FROM {node} AS n INNER JOIN {pmorganization} AS sor ON
  sor.vid=n.vid WHERE n.status=1 AND n.type='pmorganization' ORDER BY n.title";
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

  $s = "SELECT n.nid, n.title FROM {node} AS n INNER JOIN {pmticket} AS sti ON sti.vid=n.vid
  WHERE n.status=1 AND n.type='pmticket' AND sti.organization_nid=%d AND sti.project_nid=%d AND sti.task_nid=%d ORDER BY title ";
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

  $form['filter']['group2']['user'] = array(
    '#type' => 'textfield',
    '#title' => t('User'),
    '#size' => 25,
    '#default_value' => $expenseuser,
    '#autocomplete_path' => 'user/autocomplete',
  );

  $form['filter']['group3'] = array(
    '#type' => 'markup',
    '#theme' => 'pm_form_group',
  );

  $form['filter']['group3']['title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title'),
    '#size' => 40,
    '#default_value' => $title,
  );

  $form['filter']['group3']['provider_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Provider'),
    '#size' => 40,
    '#default_value' => $provider_title,
    '#autocomplete_path' => 'pm/expenses/provider_autocomplete',
  );

  $form['filter']['group4'] = array(
    '#type' => 'markup',
    '#theme' => 'pm_form_group',
  );

  $form['filter']['group4']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Filter'),
    '#submit' => array('pmexpense_list_filter_filter'),
  );

  $form['filter']['group4']['reset'] = array(
    '#type' => 'submit',
    '#value' => t('Reset'),
    '#submit' => array('pmexpense_list_filter_reset'),
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

function pmexpense_list_filter_filter($form, &$form_state) {
  $_SESSION['pmexpense_list_filter']['organization_nid'] = $form_state['values']['organization_nid'];
  $_SESSION['pmexpense_list_filter']['project_nid'] = $form_state['values']['project_nid'];
  $_SESSION['pmexpense_list_filter']['task_nid'] = $form_state['values']['task_nid'];
  $_SESSION['pmexpense_list_filter']['ticket_nid'] = $form_state['values']['ticket_nid'];
  $_SESSION['pmexpense_list_filter']['datefrom'] = $form_state['values']['datefrom'];
  $_SESSION['pmexpense_list_filter']['itemsperpage'] = $form_state['values']['itemsperpage'];
  $_SESSION['pmexpense_list_filter']['user'] = $form_state['values']['user'];
  $_SESSION['pmexpense_list_filter']['title'] = $form_state['values']['title'];
  $_SESSION['pmexpense_list_filter']['provider_title'] = $form_state['values']['provider_title'];
}

function pmexpense_list_filter_reset($form, &$form_state) {
  unset($_SESSION['pmexpense_list_filter']);
}

function pmexpense_list_form_report() {
  $form = array();
  $form['#prefix'] = '<div id="pmexpense_list_form_report" class="pm_list_form_report">';
  $form['report'] = array(
    '#type' => 'select',
    '#title' => t('Reports'),
    '#options' => theme('pmexpense_list_form_report_reports'),
  );

  if (module_exists('locale')) {
    $language_list = language_list();
    $languages = array();
    foreach ($language_list as $key => $lang) {
      $languages[$key] = $lang->name;
    }
    $form['language'] = array(
      '#type' => 'select',
      '#title' => t('Language'),
      '#options' => $languages,
    );
  }
  else {
    $form['language'] = array(
      '#type' => 'hidden',
      '#default_value' => 'en',
    );
  }
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Report'),
  );
  $form['#suffix'] = '</div>';
  return $form;
}

function pmexpense_list_form_report_submit($form, &$form_state) {
  $report = $form_state['values']['report'];
  $language = $form_state['values']['language'];
  drupal_goto('pm/expenses/report/'. $report .'/'. $language);
}

function pmexpense_list_report($report, $language) {
  $timetrackings = array();

  $s = "SELECT n.*,  sex.* FROM {node} AS n INNER JOIN {pmexpense} AS sex ON n.vid=sex.vid WHERE n.status=1 AND n.type='pmexpense' ";
  $s = pmexpense_access_sql($s, array());
  $s = db_rewrite_sql($s);

  $sql = isset($_SESSION['pmexpense_list_filter']['sql']) ? $_SESSION['pmexpense_list_filter']['sql'] : $s;
  $args = isset($_SESSION['pmexpense_list_filter']['args']) ? $_SESSION['pmexpense_list_filter']['args'] : array();
  $r = db_query($sql, $args);
  $expenses = array();
  while ($e = db_fetch_object($r)) {
    $expense = node_load($e->nid);
    $expenses[] = $expense;
  }

  print theme('pmexpense_list_report', $report, $language, $expenses);
}

function pmexpense_autocomplete($string = '') {
  // Autocomplete for providers textbox: shows any active organizations in Project Management system which have been selected as providers.
  $matches = array();
  if ($string) {
    $search = drupal_strtolower($string);

    $query = db_select('node', 'n');
    $query->join('pmorganization', 'org', 'n.vid = org.vid');
    $query
      ->fields('n', array('title'))
      ->condition('n.status', 1)
      ->condition('n.type', 'pmorganization')
      ->condition('org.isprovider', 1)
      ->condition('org.isactive', 1)
      ->where('lower(title) like :search', array(':search' => '%'. db_like($search) .'%'))
      ->addTag('node_access')
      ->range(0, 10);

    $result = $query->execute();
    foreach ($result as $pro) {
      $matches[$pro->title] = check_plain($pro->title);
    }
  }

  drupal_json_output($matches);
}

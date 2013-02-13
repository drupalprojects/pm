
function.pm.erson_organization_people(_organization_select, _person_select_id, _with_all_option, _all_text) {
  var person_select = $("#" + _person_select_id).get(0);
  pm_empty_select(person_select);
  var organization_nid = _organization_select.value;
  if (!organization_nid) organization_nid=0;

  $.ajax({
    type: "GET",
    async: true,
    url: Drupal.settings.pm.organization_people_url + Drupal.encodeURIComponent(organization_nid),
    dataType: "string",
    success: function (data) {
      var items = Drupal.parseJson(data);
      pm_fill_select(person_select, items, _with_all_option, _all_text);
    }
  });
};


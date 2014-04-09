Intend to replace all the permission handling that is carried out by PM Module.
Under Development.

Assumption.

1. There is only one single parent reference field.
2. User is Drupal user and not pmperson.


Requirement for testing.

1. A single entity reference field for referencing parent as discussed in https://drupal.org/node/2115545.
2. Entity reference field for assigning users to task/ticket/project.
3. Entity reference field for assigning users as "Project Manager" for project
4. After creating respective fields in the content type configure 'admin/config/pm/pmperm'.
5. Visit admin/people/permissions and assign permission to respective roles, the new permissions are awailable
   under "Project Management Permission".
6. Test and report error/suggestions at https://drupal.org/node/add/project-issue/pmperm

What doest work
1. PM Team
2. The "belonged" permission is a work in progress.

Experimental Feature
1. Attach an entity reference field to the pmorganization.
2. Now assign user to the organization using this field.



Update Log

20140109
  A proof of concept is ready.

20140105
  Initial release.

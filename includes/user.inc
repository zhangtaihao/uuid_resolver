<?php
// $Id: user.inc,v 1.1 2010/11/10 08:40:51 zhangtaihao Exp $
/**
 * @file
 * Implements UUID resolver on behalf of user module.
 */

/**
 * Default setting for 'use alias'.
 */
define('UUID_RESOLVER_USER_USE_ALIAS', FALSE);

/**
 * Implements hook_uuid_resolver_info().
 */
function user_uuid_resolver_info() {
  $resolvers['user'] = array(
    'title' => 'User',
    'callback' => 'uuid_resolver_resolve_user',
  );
  return $resolvers;
}

/**
 * Resolve UUID for a user.
 * @param $uuid  UUID of user.
 * @return  Path or alias to user, or nothing if none matched.
 */
function uuid_resolver_resolve_user($uuid) {
  $uid = db_result(db_query("SELECT uid FROM {uuid_users} WHERE uuid = '%s'", $uuid));
  if ($uid) {
    $path = 'user/' . $uid;
    if (variable_get('uuid_resolver_user_use_alias', UUID_RESOLVER_USER_USE_ALIAS)) {
      return drupal_get_path_alias($path);
    }
    else {
      return $path;
    }
  }
  else {
    return FALSE;
  }
}

/**
 * Implements hook_form_FORM_ID_alter() for uuid_resolver_user_settings_form().
 */
function user_form_uuid_resolver_user_settings_form_alter(&$form, &$form_state) {
  $form['#submit'][] = 'uuid_resolver_user_extra_settings_form_submit';
  $form['use_alias'] = array(
    '#title' => t('Redirect to path alias.'),
    '#description' => t('If checked, the resolver will redirect to the aliased path instead of the system path.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('uuid_resolver_user_use_alias', UUID_RESOLVER_USER_USE_ALIAS),
  );
}

/**
 * Form submit handler.
 */
function uuid_resolver_user_extra_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  variable_set('uuid_resolver_user_use_alias', (bool) $values['use_alias']);
}

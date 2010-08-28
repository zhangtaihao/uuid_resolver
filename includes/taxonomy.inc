<?php
// $Id$
/**
 * @file
 * Taxonomy UUID resolver.
 */

/** Default setting for 'use alias'. */
define('UUID_RESOLVER_TAXONOMY_TERM_USE_ALIAS', TRUE);

/**
 * Implementation of hook_uuid_resolver_info.
 */
function taxonomy_uuid_resolver_info() {
  $resolvers['taxonomy_term'] = array(
    'title' => t('Taxonomy term'),
    'callback' => 'uuid_resolver_resolve_taxonomy_term',
  );
  return $resolvers;
}

/**
 * Resolve UUID for a term.
 * @param $uuid  UUID of term.
 * @return  Path or alias to term, or nothing if none matched.
 */
function uuid_resolver_resolve_taxonomy_term($uuid) {
  $tid = db_result(db_query("SELECT tid FROM {uuid_term_data} WHERE uuid = '%s'", $uuid));
  if ($tid) {
    $path = 'taxonomy/term/'.$tid;
    if (variable_get('uuid_resolver_taxonomy_term_use_alias', UUID_RESOLVER_TAXONOMY_TERM_USE_ALIAS)) {
      return drupal_get_path_alias($path);
    } else {
      return $path;
    }
  } else {
    return FALSE;
  }
}

/**
 * Implementation of hook_form_FORM_ID_alter.
 */
function node_form_uuid_resolver_taxonomy_term_settings_form_alter(&$form, &$form_state) {
  $form['#submit'][] = 'uuid_resolver_taxonomy_term_extra_settings_form_submit';
  $form['use_alias'] = array(
    '#title' => t('Redirect to path alias.'),
    '#description' => t('If checked, the resolver will redirect to the aliased path instead of the system path.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('uuid_resolver_taxonomy_term_use_alias', UUID_RESOLVER_TAXONOMY_TERM_USE_ALIAS),
  );
}

/**
 * Form submit handler.
 */
function uuid_resolver_taxonomy_term_extra_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  variable_set('uuid_resolver_taxonomy_term_use_alias', (bool) $values['use_alias']);
}

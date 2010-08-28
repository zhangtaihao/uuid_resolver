<?php
// $Id$
/**
 * @file
 * Node UUID resolver.
 */

/** Default setting for 'use alias'. */
define('UUID_RESOLVER_NODE_USE_ALIAS', TRUE);

/**
 * Implementation of hook_uuid_resolver_info.
 */
function node_uuid_resolver_info() {
  $resolvers['node'] = array(
    'title' => t('Node'),
    'callback' => 'uuid_resolver_resolve_node',
  );
  $resolvers['node_revision'] = array(
    'title' => t('Node revision'),
    'callback' => 'uuid_resolver_resolve_node_revision',
  );
  return $resolvers;
}

/**
 * Resolve UUID for a node path.
 * @param $uuid  UUID of node.
 * @return  Path or alias to node, or nothing if none matched.
 */
function uuid_resolver_resolve_node($uuid) {
  $nid = db_result(db_query("SELECT nid FROM {uuid_node} WHERE uuid = '%s'", $uuid));
  if ($nid) {
    $path = 'node/'.$nid;
    if (variable_get('uuid_resolver_node_use_alias', UUID_RESOLVER_NODE_USE_ALIAS)) {
      return drupal_get_path_alias($path);
    } else {
      return $path;
    }
  }
}

/**
 * Resolve UUID for a node revision path.
 * @param $uuid  UUID of node revision.
 * @return  Path to node revision, or nothing if none matched.
 */
function uuid_resolver_resolve_node_revision($uuid) {
  $match = db_fetch_array(db_query("SELECT v.nid, v.vid FROM
      {uuid_node_revisions} u LEFT JOIN {node_revisions} v ON u.vid = v.vid
      WHERE uuid = '%s'", $uuid));
  if ($match) {
    return 'node/'.$match['nid'].'/revisions/'.$match['vid'].'/view';
  }
}

/**
 * Implementation of hook_form_FORM_ID_alter.
 */
function node_form_uuid_resolver_node_settings_form_alter(&$form, &$form_state) {
  $form['#submit'][] = 'uuid_resolver_node_extra_settings_form_submit';
  $form['use_alias'] = array(
    '#title' => t('Redirect to path alias.'),
    '#description' => t('If checked, the resolver will redirect to the aliased path instead of the system path.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('uuid_resolver_node_use_alias', UUID_RESOLVER_NODE_USE_ALIAS),
  );
}

/**
 * Form submit handler.
 */
function uuid_resolver_node_extra_settings_form_submit($form, $form_state) {
  $values = $form_state['values'];
  variable_set('uuid_resolver_node_use_alias', (bool) $values['use_alias']);
}

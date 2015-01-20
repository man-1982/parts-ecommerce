<?php
/**
 * Created by PhpStorm.
 * User: kochergin
 * Date: 15.01.15
 * Time: 14:16
 */


function sm_generator_load_texts($form, &$form_state, $term) {
  $fetchasoc = sm_generator_check_keys($term->tid);
  $term_entity_wrapper = entity_metadata_wrapper('taxonomy_term', $term);
//  dpm($term_entity_wrapper->field_templates->value->value());
  if(!$fetchasoc) {
    $form['message'] = array(
      '#markup'    => 'Нет сгенерированных текстов',
    );
    return ($form);
  }
  $texts = unserialize($fetchasoc['texts']);
  $i = 0;
  $form['template'] = array(
    '#type'   => 'item',
    '#title'  => 'Исходный шаблон',
    '#markup'   => $term_entity_wrapper->field_template->value(),
  );
  foreach($texts as $hash => $text) {
//    if (module_exists('devel')) {
//      dpm($text, $hash);
//    }
    $i++;
    $form[$hash] = array(
      '#type' => 'item',
      '#title' => "# {$i} " . 'hash: ' .  $hash,
      '#markup' => $text
    );
    //dpm($text, $hash);
  }
  return $form;

}
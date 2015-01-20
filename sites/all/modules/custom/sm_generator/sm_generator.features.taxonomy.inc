<?php
/**
 * @file
 * sm_generator.features.taxonomy.inc
 */

/**
 * Implements hook_taxonomy_default_vocabularies().
 */
function sm_generator_taxonomy_default_vocabularies() {
  return array(
    'applicability_parts' => array(
      'name' => 'Применимость автозапчасти',
      'machine_name' => 'applicability_parts',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
      'uuid' => 'c589517e-f7e8-495d-a3e8-aca66cb603c9',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
    'autospare_parts' => array(
      'name' => 'Замена автозапчасти',
      'machine_name' => 'autospare_parts',
      'description' => 'Замена автозапчасти',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => -5,
      'uuid' => 'b3340daf-5bd5-4f44-ace4-4f348019c7e3',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
    'decription_parts' => array(
      'name' => 'Описание запчасти',
      'machine_name' => 'decription_parts',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
      'uuid' => '55e6095c-4753-4e3a-a14e-b7c4c43ac383',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
    'remanufactured_parts' => array(
      'name' => 'Восстановленные запчасти',
      'machine_name' => 'remanufactured_parts',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => -6,
      'uuid' => '26ef5fbc-3255-4741-b0b7-8194047982fa',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
    'structural_numbers' => array(
      'name' => 'Конструкционные номера',
      'machine_name' => 'structural_numbers',
      'description' => '',
      'hierarchy' => 0,
      'module' => 'taxonomy',
      'weight' => 0,
      'uuid' => '1b210acc-495b-4dfa-bee9-f3963b60d7fb',
      'rdf_mapping' => array(
        'rdftype' => array(
          0 => 'skos:ConceptScheme',
        ),
        'name' => array(
          'predicates' => array(
            0 => 'dc:title',
          ),
        ),
        'description' => array(
          'predicates' => array(
            0 => 'rdfs:comment',
          ),
        ),
      ),
    ),
  );
}

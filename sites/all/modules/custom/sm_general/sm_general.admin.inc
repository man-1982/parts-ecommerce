<?php
/**
 * Created by PhpStorm.
 * User: kochergin
 * Date: 08.01.15
 * Time: 15:55
 */


function sm_general_get_brands_model($form, &$form_state) {

  $form['wr_submit_get_brands'] = array(
    '#type'         => 'fieldset',
    '#description'  => 'Заполняем список моделей на основе словаря Brand (Марки). Внимаение: Операция длительная и перед ее выполнением нужно заполнить марки'
  );

  $form['wr_submit_get_brands']['submit_get_brands'] = array(
    '#type'         => 'submit',
    '#value'        => 'Заполнить/обновить список брендов',
    '#submit'       => array('sm_general_get_marks_submit'),
    '#description'  => 'Заполняем список брендов/марок с сайта 2wipe.ru. Эта информациянеобходима чтобы заполнить списко моделей'
  );

  $form['wr_submit_get_model_by_brand'] = array(
    '#type'         => 'fieldset',
    '#description'  => 'Заполняем список моделей на основе словаря Brand (Марки). Внимаение: Операция длительная и перед ее выполнением нужно заполнить марки'
  );

  $form['wr_submit_get_model_by_brand']['submit_get_model_by_brand'] = array(
    '#type'    => 'submit',
    '#value'   => 'Заполнить/обновить список моделей по брендам автомобилей',
    '#submit'  => array('sm_general_get_model_by_brand_submit'),

  );

  $form['wr_submit_get_type_by_model'] = array(
    '#type'         => 'fieldset',
    '#description'  => 'Заполняем типы моделей - это более подробная информация по моделям.'
  );
  $form['wr_submit_get_type_by_model']['submit_get_type_by_model'] = array(
    '#type'    => 'submit',
    '#value'   => 'Заполнить обновить список типов моделей',
    '#submit'  => array('sm_general_get_type_by_mode_submit'),

  );

  $form['wr_submit_get_catalog_by_model'] = array(
    '#type'         => 'fieldset',
    '#description'  => 'Заполняем каталог, каталог заполняется на основании моделей. Поэтому перед заполнение каталога нужно заполнить модели.'
  );

  $form['wr_submit_get_catalog_by_model']['submit_get_сatalog_by_model'] = array(
    '#type'    => 'submit',
    '#value'   => 'Заполнить "Каталог"',
    '#submit'  => array('sm_general_get_catalog_by_mode_submit'),

  );

  return $form;
}

/**
 * @param $form
 * @param $form_state
 */
function sm_general_get_marks_submit(&$form, &$form_state) {
  $server = new SMGeneralSoap();
  $list_of_auto_brands = $server->AutoBrands();
  $voc = taxonomy_vocabulary_machine_name_load("marks_auto");
  drupal_set_message('List saved terms');
  foreach($list_of_auto_brands as $item) {
    $name = $item->BrandName; // наименование бренда на английском
    $brandimageid = empty($item->BrandImageId) ? '' : $item->BrandImageId;
    $branddescription = empty($item->BrandDescription) ? '' : $item->BrandDescription; // наименование бренда (марки) на русском
    $vehicletype = empty($item->VehicleType) ? "" : $item->VehicleType;

    if(empty($brandimageid) || empty($branddescription) || empty($vehicletype)) {
//      dpm($item);
    }

    $term_brand = sm_general_get_term_brand_by_name($name);
    if(empty($term_brand)) {
      $term = entity_create('taxonomy_term', array(
        'name' => $name,
        'vid' => $voc->vid,
        'description' => $branddescription
      ));
      taxonomy_term_save($term);
      _sm_general_brands_crud($term, $branddescription, $brandimageid, $vehicletype);
    }
    else {
      $terms = array($term_brand);
      foreach($terms as $term) {
        _sm_general_brands_crud($term, $branddescription, $brandimageid, $vehicletype);
      }
    }
  }
}

/**
 * heper function for crud taxonomy term vocabularies marks_auto
 * @param $name
 * @param $voc
 * @param $branddescription
 * @param $brandimageid
 * @param $vehicletype
 */
function _sm_general_brands_crud($term, $branddescription, $brandimageid, $vehicletype) {
  $term_wrapper = entity_metadata_wrapper('taxonomy_term', $term);
  try {
    $term_wrapper->field_brandimageid = $brandimageid;
    $term_wrapper->field_branddescription = $branddescription;
    $term_wrapper->field_vehicletype = $vehicletype;
    $term_wrapper->field_brandname = $term->name;
    $term_wrapper->save();
    drupal_set_message($term->name );
  }
  catch ( exception $e ) {

    watchdog('SM:MODELS', 'Ошика для ' .$term_wrapper->name->value() . ' : '. $term->tid . ' Ошибка:'  .$e->getMessage());
  }

}

/**
 *
 * @param $form
 * @param $form_state
 */
function sm_general_get_model_by_brand_submit(&$form, &$form_state) {
  $server = new SMGeneralSoap();
  $voc_brands = taxonomy_vocabulary_machine_name_load("marks_auto");
  $voc_model  = taxonomy_vocabulary_machine_name_load("model");
  $tree_brands = taxonomy_get_tree($voc_brands->vid, 0, NULL, TRUE);
  $batch = array();
  foreach($tree_brands as $brand) {
    $brand_wr = entity_metadata_wrapper('taxonomy_term', $brand);
    $brand_name = $brand_wr->field_brandname->value();

    //получаем модели по брендам
    if(empty($brand_name)) {
      drupal_set_message("Для термина {$brand->name} не найден/установлено имя бренда", 'error');
      drupal_set_message("Заполните/обновите список брендов заново", 'error');
      continue;
    }
    // получаем список моделей по бренду
    $result = $server->AutoModelByBrand($brand_name);

    foreach($result as $item) {
      $ModelName      = $item->ModelName;
      if(empty($ModelName)) {
        dpm($item);
        continue;
      }
      $ModelId        = $item->ModelId;
      $ModelImageId   = $item->ModelImageId;
      $VehicleType    = $item->VehicleType;
      $ModelDateFrom  = $item->ModelDateFrom;
      $ModelDateTo    = $item->ModelDateTo;

//      dpm($item, $item->ModelName);
      $term_model = sm_general_get_term_model_by_name($ModelName);

      if(empty($term_model)) {
        $term_model = entity_create('taxonomy_term', array('name' => $ModelName, 'vid' => $voc_model->vid));
        taxonomy_term_save($term_model);
      }

      $vars = compact('term_model', 'ModelId', 'ModelImageId', 'VehicleType', 'ModelName', 'ModelDateFrom', 'ModelDateTo', 'brand');
      // create model
      $batch['operations'][] = array('_sm_general_model_crud', array($vars));
      //_sm_general_model_crud($vars);

    }
  }
  $batch['file'] = drupal_get_path('module', 'sm_general') . '/sm_general.admin.inc';
  batch_set($batch);
  batch_process();
}


/**
 * heper function for crud taxonomy term vocabularies marks_auto
 * @param $name
 * @param $voc
 * @param $branddescription
 * @param $brandimageid
 * @param $vehicletype
 */
function _sm_general_model_crud($vars) {

  $term_model     = $vars['term_model'];
  $ModelId        = $vars['ModelId'];
  $ModelImageId   = $vars['ModelImageId'];
  $VehicleType    = $vars['VehicleType'];
  $ModelName      = $vars['ModelName'];
  $brand          = $vars['brand'];

  try {
    $term_wrapper = entity_metadata_wrapper('taxonomy_term', $term_model);
    $term_wrapper->field_modelid           = $ModelId;
    $term_wrapper->field_modelimageid      = $ModelImageId;
    $term_wrapper->field_vehicletype       = $VehicleType;
    $term_wrapper->field_modelname         = $ModelName;
    $term_wrapper->field_modeldatefrom    = !empty($ModelDateFrom) ? strtotime($ModelDateFrom): 0;
    $term_wrapper->field_modeldateto       = !empty($ModelDateTo) ? strtotime($ModelDateTo): 0;
    $term_wrapper->field_term_reference_id  = $brand->tid;
    $term_wrapper->save();
    drupal_set_message("Создана/Обновлена модель " . $term_model->name );
  }
  catch (Exception $e) {
    drupal_set_message('Ошибка при создании/обновлении модели ' .$term_model->name . ' Ошибка:'  .$e->getMessage() . '', 'error');
    watchdog('SM:MODELS', 'Ошика для ' .$term_wrapper->name->value() . ' : '. $term_model->tid . ' Ошибка:'  .$e->getMessage());
  }
}

/**
 * Callback function for create update type of models
 * @param $form
 * @param $form_state
 */
function sm_general_get_type_by_mode_submit(&$form, &$form_state) {
  $server       = new SMGeneralSoap();
  $voc_model    = taxonomy_vocabulary_machine_name_load("model");
  $voc_type    = taxonomy_vocabulary_machine_name_load("model_types");
  $tree_model  = taxonomy_get_tree($voc_model->vid, 0, NULL, TRUE);
  $batch = array();
  foreach($tree_model as $model) {
    $wr_model = entity_metadata_wrapper('taxonomy_term', $model);
    if(empty($wr_model->field_modelid)) {
      continue;
    }

    // batch processs for the extract info about of models type
    $batch['operations'][] = array('_sm_types_by_model_extendent_info', array($server, $wr_model, $model, $voc_type));
  }

  $batch['file'] = drupal_get_path('module', 'sm_general') . '/sm_general.admin.inc';
  batch_set($batch);
  batch_process();

}

/**
 * @param $server
 * @param $wr_model
 * @param $model
 * @param $voc_type
 * @param $batch
 * @return mixed
 */
function _sm_types_by_model_extendent_info($server, $wr_model, $model, $voc_type) {
  $types_list = $server->AutoCatalogTypesGet($wr_model->field_modelid->value());

  $vars = array();
  $batch = array();
  foreach ($types_list as $type_info) {
    $model_id = empty($type_info->ModelId) ? 0 : $type_info->ModelId;
    if (empty($model_id)) {
      continue;
    }
    $vars['field_term_ref_model_id'] = $model->tid;
    $vars['vid'] = $voc_type->vid;
    $vars['field_modelid'] = $model_id;
    $vars['field_typeid'] = $type_info->TypeId;
    $vars['field_typeengine'] = $type_info->TypeEngine;
    $vars['field_typeenginevalueliters'] = $type_info->TypeEngineValueLiters;
    $vars['field_typeenginevalueccm'] = $type_info->TypeEngineValueCcm;
    $vars['field_typeenginepowerhp'] = $type_info->TypeEnginePowerHp;
    $vars['field_typepowerkw'] = $type_info->TypeEnginePowerKw;
    $vars['field_typeenginetype'] = $type_info->TypeEngineType;
    $vars['field_typefuel'] = $type_info->TypeFuel;
    $vars['field_typefueltype'] = $type_info->TypeFuelType;
    $vars['field_typedrive'] = $type_info->TypeDrive;
//      $vars['field_typedoors']              = $type_info->TypeDoors;
    $vars['field_typebody'] = $type_info->TypeBody;
    $vars['field_typecylinders'] = $type_info->TypeCylinders;
    $vars['field_modeldatefrom'] = empty($type_info->TypeDateFrom) ? 0 : strtotime($type_info->TypeDateFrom);
    $vars['field_modeldateto'] =  empty($type_info->TypeDateTo) ? 0 : strtotime($type_info->TypeDateTo); ;

    _sm_general_models_type_crud($vars);

  }
}


/**
 * Heplep function for create updatre models type
 */
function _sm_general_models_type_crud($vars) {
  $model_id = $vars['field_modelid'];
  $type_id  = $vars['field_typeid'];
//  $term_wrapper->field_modeldatefrom    = !empty($ModelDateFrom) ? strtotime($ModelDateFrom): 0;
//  $term_wrapper->field_modeldateto       = !empty($ModelDateTo) ? strtotime($ModelDateTo): 0;


  $list_types = sm_general_get_type_by_model($model_id, $type_id, TRUE);
  if(empty($list_types)) {
    $type_term = entity_create('taxonomy_term', array('name' => $type_id, 'vid' => $vars['vid']));
    unset($vars['vid']);
    taxonomy_term_save($type_term);
    $list_types[] = $type_term;
  }
  // type is term object for models types
  foreach($list_types as $type) {
    $term_wrapper = entity_metadata_wrapper('taxonomy_term', $type);

    foreach($vars as $key => $value) {
      try {
        $term_wrapper->{$key}  = $value;
      }
      catch (Exception $e) {
        drupal_set_message('Создана/Обновлена тип модели ' . $term_wrapper->name->value() . ' Ошибка:'  .$e->getMessage() . '', 'error');
        watchdog('SM:TYPES', 'Ошибка для name: ' . $term_wrapper->name->value() . ' Type tid: '. $type->tid . ' Model tid:'. $model_id  .' Ошибка:'  .$e->getMessage());
      }
    }

    $term_wrapper->save();

  }

}

/**
 *
 */
function sm_general_get_catalog_by_mode_submit(&$form, &$form_state) {
  $voc_model    = taxonomy_vocabulary_machine_name_load("model");
  $tree_model  = taxonomy_get_tree($voc_model->vid, 0, NULL, TRUE);
  $voc_catalog = taxonomy_vocabulary_machine_name_load('catalog');
  $server       = new SMGeneralSoap();
  $batch = array();
  foreach($tree_model as $model) {
//    _sm_general_catalog_crud_batch_oparations($model, $server, $voc_catalog);

    // batch processs for the extract info about of models type
    $batch['operations'][] = array('_sm_general_catalog_crud_batch_oparations', array($model, $server, $voc_catalog));
  }

  $batch['file'] = drupal_get_path('module', 'sm_general') . '/sm_general.admin.inc';
  batch_set($batch);
  batch_process();

}

/**
 * @param $model
 * @param $server
 * @param $vars
 * @param $voc_catalog
 */
function _sm_general_catalog_crud_batch_oparations($model, $server, $voc_catalog) {
  $wr_model = entity_metadata_wrapper('taxonomy_term', $model);
  $model_id = $wr_model->field_modelid->value();
    if(empty($model_id)) {
      return;
    }
  $vars = array();
  $result = $server->AutoCatalogSearchTreeGet($model_id);
  $vars['result'] = $result;
  $vars['vid'] = $voc_catalog->vid;
  $vars['modelid'] = $model_id;
  $vars['parent'][0] = 0;
  _sm_general_catalog_crud($vars);
}


function _sm_general_catalog_crud($vars) {
  if(empty($vars['result'])) {
    return ;
  }
  $result = $vars['result'];
  foreach($result as $item) {
    $field_nodeid       = $item->NodeId;
    $field_nodename     = $item->NodeName;
    $field_nodeimageid  = $item->NodeImageId;
    if(empty($field_nodeid) || empty($field_nodename)) {
      continue;
    }
    $fields = compact('field_nodeid', 'field_nodename', 'field_nodeimageid');
    $name               = $field_nodename;
    $vid                = $vars['vid'];
    $catalog_terms = sm_general_get_catalog_by_nodeid($field_nodeid, TRUE);
    if(empty($catalog_terms)) {
      $term = entity_create('taxonomy_term', array('vid' => $vid, 'name' => $name));
      taxonomy_term_save($term);
      $catalog_terms[] = $term;
    }

    $term = array();
    foreach($catalog_terms as $term) {
      $term_wr_catalog = null;
      $term_wr_catalog = entity_metadata_wrapper('taxonomy_term', $term);

      try {
        foreach($fields as $key => $value) {
          $term_wr_catalog->{$key} = $value;
        }
        $term_wr_catalog->parent = $vars['parent'];
        $term_wr_catalog->save();
      }
      catch( Exception $e) {
        watchdog('SM:CATALOG', 'Каталог tid: ' . $term->tid . ' NodeId:' . $field_nodeid . ' Name:' . $term->name . ' Ошибка:' . $e->getMessage());
      }


      if(!empty($item->ChildNodes->CatalogTreeApiView)) {
        $vars_new = array();
        $vars_new['result'] = $item->ChildNodes->CatalogTreeApiView;
        if(!empty($vars_new['result']->NodeId) && !empty($vars_new['result']->NodeName)) {
          $item_result = $vars_new['result'];
          unset($vars_new['result']);
          $vars_new['result'][] = $item_result;
          $a = 1;
        }
        $vars_new['vid']        = $vid;
        $vars_new['parent'][]     =  $term->tid;
        _sm_general_catalog_crud($vars_new);
      }
    }


  }
}




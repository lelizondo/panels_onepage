<?php

function panels_onepage_customize_step_two($form, &$form_state, $node) {
	global $user;

	if(isset($node->panels_onepage)) {
		$sections = $node->panels_onepage;
	}

	if($sections["total"] > 0) {
		$form["_sections"] = array("#type" => "hidden", "#value" => $sections["total"]);

		$form["_nid"] = array(
			'#type' => "hidden",
			"#value" => $node->nid
		);
		
		// Crear una lista de 3 columnas máximo
		$cols = array();
		for($c=1; $c <= 3; $c++) {
			$cols[$c] = $c;
		}

		// // Cuantas secciones vamos a crear
		for($i=1; $i <= $sections["total"]; $i++) {
			$form["well-" . $i] = array(
				'#prefix' => '<div class="panels-onepage-step-two-well well">',
				'#suffix' => '</div>'
			);

			$form["well-" . $i]["container-" . $i] = array(
				"#type" => "fieldset",
				'#title' => t("Settings for section !section", array("!section" => $i)),
			);

			$form["well-" . $i]["container-" . $i]["menu-title-" . $i] = array(
				'#type' => 'textfield',
				'#title' => t("Menu title"),
				'#required' => FALSE,
				"#description" => t("Used to set the menu title"),
				"#default_value" => isset($node->panels_onepage["sections"][$i]["menu_title"]) ? $node->panels_onepage["sections"][$i]["menu_title"] : NULL
			);

			$form["well-" . $i]["container-" . $i]["section-background-color-" . $i] = array(
				'#type' => 'textfield',
				'#title' => t("Section background color"),
				'#required' => FALSE,
				"#description" => t("Don't add the #"),
				"#default_value" => isset($node->panels_onepage["sections"][$i]["backgroundColor"]) ? $node->panels_onepage["sections"][$i]["backgroundColor"] : NULL
			);			

			$form["well-" . $i]["container-" . $i]["section-cols-" . $i] = array(
				'#type' => 'select',
				'#title' => t("How many columns you need in this section?"),
				'#required' => TRUE,
				'#description' => t("Select how many columns you need for this section"),
				'#options' => $cols,
				"#default_value" => isset($node->panels_onepage["sections"][$i]["cols"]) ? $node->panels_onepage["sections"][$i]["cols"] : NULL
			);

			$form["well-" . $i]["container-" . $i]["section-height-" . $i] = array(
				'#type' => 'textfield',
				'#title' => t("Section minimum height"),
				'#required' => FALSE,
				"#description" => t("add px or em"),
				"#default_value" => isset($node->panels_onepage["sections"][$i]["height"]) ? $node->panels_onepage["sections"][$i]["height"] : NULL
			);	

			$form["well-" . $i]["container-" . $i]["section-image-" . $i] = array(
				'#type' => 'managed_file',
				'#title' => t("Background image"),
				'#required' => FALSE,
				'#description' => t("If you want to display a background image, you can upload it here"),
				'#upload_location' => 'public://panels_onepage/',
				'#default_value' => isset($node->panels_onepage["sections"][$i]["backgroundImage"]) ? $node->panels_onepage["sections"][$i]["backgroundImage"] : NULL
			);
		}

		$form["actions"] = array(
			'#prefix' => '<div class="actions">',
			'#suffix' => '</div>'
		);

		$form["actions"]["submit"] = array(
			"#type" => "submit",
			"#value" => t("Next")
		);
	}
	
	return $form;
}

function panels_onepage_customize_step_two_submit(&$form, &$form_state) {
	global $user;

	$values = $form_state["values"];
	$sections = $values["_sections"];
	$nid = $values["_nid"];

	$ret = array();

	$ret["total"] = $sections;
	$ret["sections"] = array();
	for($i = 1; $i <= $sections; $i++) {
		$ret["sections"][$i] = array(
			'cols' => $values["section-cols-" . $i],
			'menu_title' => check_plain($values["menu-title-" . $i]),
			'height' => check_plain($values["section-height-" . $i]),
			'backgroundColor' => check_plain($values["section-background-color-" . $i]),
		);
		
		$file = file_load($values["section-image-" . $i]);
		if($file) {
			$file->status = FILE_STATUS_PERMANENT;
			file_save($file);
			file_usage_add($file, "panels_onepage", "node", $nid);
			$ret["sections"][$i]["backgroundImage"] = $file->fid;
		}
	}

	// save here
	panels_onepage_save($nid, $ret);

	$node = node_load($nid);
	if(isset($node->panelizer)) {
		if($node->panelizer["page_manager"]->pipeline == "ipe") {
			$form_state["redirect"] = "node/$nid";
		}
		else {
			$form_state["redirect"] = "node/$nid/panelizer/page_manager/content";
		}
	}
}
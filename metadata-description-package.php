<?php
include_once("includes/metadata-description-package-header.php");


// Settings
//

// Get current date
$currentDate = date("d/m/Y");

// Set input field size
$fieldWidth = 300;

// Set label font size
$labelFontSize = 14;

// Set text font size
$textFontSize = 14;


// Define groups
//

$identityStatementGroup = new MetadataTemplateItemGroup("identity_statement", "Identity statement");
$contextGroup = new MetadataTemplateItemGroup("context", "Context");
$contentAndStructureGroup = new MetadataTemplateItemGroup("content_and_structure", "Content and structure");
$conditionsOfAccessAndUseGroup = new MetadataTemplateItemGroup("conditions_of_access_and_use", "Conditions of access and use");
$alliedMaterialsGroup = new MetadataTemplateItemGroup("allied_materials", "Allied materials");
$noteGroup = new MetadataTemplateItemGroup("note", "Note");


// Define metadata
//

// Reference code(s)
$item = new MetadataTemplateItemString("reference_codes", "Reference code(s)", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$identityStatementGroup->addItem($item);

// Title
$item = new MetadataTemplateItemString("title", "Title", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$identityStatementGroup->addItem($item);

// Dates of creation of the material in the unit of description
$item = new MetadataTemplateItemDatetime("dates_of_creation_of_the_material_in_the_unit_of_description", "Dates of creation of the \nmaterial in the unit of description", $currentDate);
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$item->setFormat("dd/MM/yyyy");
$item->setMaxDate(date("Y"), date("m"), date("d"));
$item->setMinDate("500", date("01"), date("01"));
$item->enableCalendar(true);
$identityStatementGroup->addItem($item);

// Level of description
$item = new MetadataTemplateItemDropdown("level_of_description", "Level of description", "fonds");
$item->addOption("Fonds", "fonds");
$item->addOption("Series", "series");
$item->addOption("Sub-series", "subseries");
$item->addOption("File", "file");
$item->addOption("Item", "item");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$identityStatementGroup->addItem($item);

// Extent of the unit of description
$item = new MetadataTemplateItemString("extent_of_the_unit_of_description", "Extent of the unit of description", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$identityStatementGroup->addItem($item);


// Name of creator
$item = new MetadataTemplateItemString("name_of_creator", "Name of creator", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contextGroup->addItem($item);

// Administrative / Biographical history
$item = new MetadataTemplateItemString("administrative_biographical_history", "Administrative / \nBiographical history", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contextGroup->addItem($item);

// Dates of accumulation of the unit of description
$item = new MetadataTemplateItemDatetime("dates_of_accumulation_of_the_unit_of_description", "Dates of accumulation of\nthe unit of description", $currentDate);
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$item->setFormat("dd/MM/yyyy");
$item->setMaxDate(date("Y"), date("m"), date("d"));
$item->setMinDate("500", date("01"), date("01"));
$item->enableCalendar(true);
$contextGroup->addItem($item);

// Custodial history
$item = new MetadataTemplateItemString("custodial_history", "Custodial history", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contextGroup->addItem($item);

// Immediate source of acquisition
$item = new MetadataTemplateItemString("immediate_source_of_acquisition", "Immediate source of acquisition", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contextGroup->addItem($item);


// Scope and content / Abstract
$item = new MetadataTemplateItemString("scope_and_content_abstract", "Scope and content / Abstract", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contentAndStructureGroup->addItem($item);

// Appraisal, destruction and scheduling information
$item = new MetadataTemplateItemString("appraisal_destruction_and_scheduling_information", "Appraisal, destruction and\nscheduling information", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contentAndStructureGroup->addItem($item);

// Accruals
$item = new MetadataTemplateItemString("accruals", "Accruals", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contentAndStructureGroup->addItem($item);

// System of arrangement
$item = new MetadataTemplateItemString("system_of_arrangement", "System of arrangement", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$contentAndStructureGroup->addItem($item);


// Legal status
$item = new MetadataTemplateItemString("legal_status", "Legal status", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$conditionsOfAccessAndUseGroup->addItem($item);

// Access conditions
$item = new MetadataTemplateItemString("access_conditions", "Access conditions", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$conditionsOfAccessAndUseGroup->addItem($item);

// Copyright / Conditions governing reproduction
$item = new MetadataTemplateItemString("copyright_conditions_governing_reproduction", "Copyright / Conditions\ngoverning reproduction", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$conditionsOfAccessAndUseGroup->addItem($item);

// Language of material
$item = new MetadataTemplateItemDropdown("language_of_material", "Language of material", "English");
foreach ($languages as $language)
{
    $item->addOption($language, $language);
}
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$conditionsOfAccessAndUseGroup->addItem($item);

// Physical characteristics
$item = new MetadataTemplateItemString("physical_characteristics", "Physical characteristics", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$conditionsOfAccessAndUseGroup->addItem($item);

// Finding aids
$item = new MetadataTemplateItemString("finding_aids", "Finding aids", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$conditionsOfAccessAndUseGroup->addItem($item);


// Location of originals
$item = new MetadataTemplateItemString("location_of_originals", "Location of originals", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$alliedMaterialsGroup->addItem($item);

// Existence of copies
$item = new MetadataTemplateItemString("existence_of_copies", "Existence of copies", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$alliedMaterialsGroup->addItem($item);

// Related units of description
$item = new MetadataTemplateItemString("related_units_of_description", "Related units of description", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$alliedMaterialsGroup->addItem($item);

// Associated material
$item = new MetadataTemplateItemString("associated_material", "Associated material", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$alliedMaterialsGroup->addItem($item);

// Publication note
$item = new MetadataTemplateItemString("publication_note", "Publication note", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$alliedMaterialsGroup->addItem($item);


// Note
$item = new MetadataTemplateItemString("note", "Note", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$noteGroup->addItem($item);


// Add groups
//

$metadataTemplate->addGroup($identityStatementGroup);
$metadataTemplate->addGroup($contextGroup);
$metadataTemplate->addGroup($contentAndStructureGroup);
$metadataTemplate->addGroup($conditionsOfAccessAndUseGroup);
$metadataTemplate->addGroup($alliedMaterialsGroup);
$metadataTemplate->addGroup($noteGroup);

include_once("includes/metadata-description-package-footer.php");
?>

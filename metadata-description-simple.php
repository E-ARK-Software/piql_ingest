<?php
include_once("includes/error-handler.php");
include_once("includes/metadata-description-header.php");

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

// Groups
//

// File metadata
$fileMetadataGroup = new MetadataTemplateItemGroup("filemetadata", "File metadata");

// Metadata
//

// Title
$item = new MetadataTemplateItemString("title", "Title", $fileName);
$item->setWidth($fieldWidth);
$item->setReadOnly(true);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Date
$item = new MetadataTemplateItemDatetime("date", "Date", $currentDate);
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$item->setFormat("dd/MM/yyyy");
$item->setMaxDate(date("Y"), date("m"), date("d"));
$item->setMinDate("500", date("01"), date("01"));
$item->enableCalendar(true);
$fileMetadataGroup->addItem($item);

// ATOM
$item = new MetadataTemplateItemDropdown("description", "Atom", "aeronauticos");
$item->addOption("Aeronauticos", "aeronauticos");
$item->addOption("Recursos Humanos", "recursos-humanos");
$item->addOption("Contratos", "contratos");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Provenance
$item = new MetadataTemplateItemString("provenance","Provenance", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Creator
$item = new MetadataTemplateItemString("creator", "Creator", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Subject
$item = new MetadataTemplateItemString("subject", "Subject", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Publisher
$item = new MetadataTemplateItemString("publisher", "Publisher", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$item->validateRegex("...*"); // Must contain at least two characters
$fileMetadataGroup->addItem($item);

// Type
$item = new MetadataTemplateItemString("type", "Type", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Identifier
$item = new MetadataTemplateItemString("identifier", "Identifier", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Source
$item = new MetadataTemplateItemString("source", "Source", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Language
$item = new MetadataTemplateItemString("language", "Language", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Relation
$item = new MetadataTemplateItemString("relation", "Relation", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Format
$item = new MetadataTemplateItemString("format", "Format", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Coverage
$item = new MetadataTemplateItemString("coverage", "Coverage", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Rights
$item = new MetadataTemplateItemString("rights", "Rights", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Contributor
$item = new MetadataTemplateItemString("contributor", "Contributor", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$fileMetadataGroup->addItem($item);

// Add groups
$metadataTemplate->addGroup($fileMetadataGroup);

include_once("includes/metadata-description-footer.php");
?>

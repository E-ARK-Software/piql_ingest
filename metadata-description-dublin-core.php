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


// Define groups
//

$metadataGroup = new MetadataTemplateItemGroup("metadata", "Metadata");

// Define metadata
//

// Contributor
$item = new MetadataTemplateItemString("contributor", "Contributor", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Coverage
$item = new MetadataTemplateItemString("coverage", "Coverage", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Creator
$item = new MetadataTemplateItemString("creator", "Creator", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

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
$metadataGroup->addItem($item);

// Description
$item = new MetadataTemplateItemString("description", "Description", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Format
$item = new MetadataTemplateItemString("format", "Format", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Identifier
$item = new MetadataTemplateItemString("identifier", "Identifier", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Language
$item = new MetadataTemplateItemDropdown("language", "Language", "English");
foreach ($languages as $language)
{
    $item->addOption($language, $language);
}
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Publisher
$item = new MetadataTemplateItemString("publisher", "Publisher", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Relation
$item = new MetadataTemplateItemString("relation", "Relation", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Rights
$item = new MetadataTemplateItemString("rights", "Rights", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Source
$item = new MetadataTemplateItemString("source", "Source", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Subject
$item = new MetadataTemplateItemString("subject", "Subject", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Title
$item = new MetadataTemplateItemString("title", "Title", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Type
$item = new MetadataTemplateItemString("type", "Type", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Add groups
//

$metadataTemplate->addGroup($metadataGroup);


include_once("includes/metadata-description-footer.php");
?>

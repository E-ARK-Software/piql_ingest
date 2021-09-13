<?php
include_once("includes/error-handler.php");
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

$metadataGroup = new MetadataTemplateItemGroup("metadata", "Metadata");


// Define metadata
//

// Software version
$item = new MetadataTemplateItemString("softwareVersion", "Software version", "1.1.0");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Submission agreement
$item = new MetadataTemplateItemString("submissionagreement", "Path or URL to Submission Agreement", "\\documentation");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Reference code
$item = new MetadataTemplateItemString("referencecode", "Submission agreement reference code", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Previous submission agreement
$item = new MetadataTemplateItemString("previoussubmissionagreement", "Path or URL to Previous Submission Agreement", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Previous reference code
$item = new MetadataTemplateItemString("previousreferencecode", "Previous submission agreement reference code", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Creator name
$item = new MetadataTemplateItemString("creatorName", "Creator organisation name", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Creator ID
$item = new MetadataTemplateItemString("creatorID", "Creator organisation identifier", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Archive name
$item = new MetadataTemplateItemString("archiveName", "Archive organisation name", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Archive ID
$item = new MetadataTemplateItemString("archiveID", "Archive identifier", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Preservation
$item = new MetadataTemplateItemString("preservation", "Preservation organisation name", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Preservation ID
$item = new MetadataTemplateItemString("preservationID", "Preservation organisation identifier", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Submitter name
$item = new MetadataTemplateItemString("submitterName", "Submitter name", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Submitter details
$item = new MetadataTemplateItemString("submitterdetails", "Submitter details", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Patient schema name
$item = new MetadataTemplateItemString("patientschemaName", "Patient Personal Information schema name", "Patients.xsd");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Patient schema data link
$item = new MetadataTemplateItemString("patientschemaDataLink", "Path or URL to Patient Personal Information schema", "/schemas/Patients.xsd");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Patient clinical schema name
$item = new MetadataTemplateItemString("patientclinicalschemaName", "Patient Clincal Information schema name", "condition.xsd");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Patient clinical schema data link
$item = new MetadataTemplateItemString("patientclinicalschemaDataLink", "Patient clinical schema data link URL", "");
$item->setWidth($fieldWidth);
$item->setReadOnly(false);
$item->setVisible(true);
$item->setLabelFontSize($labelFontSize);
$item->setFontSize($textFontSize);
$metadataGroup->addItem($item);

// Add groups
//

$packageMetadataTemplate->addGroup($metadataGroup);

include_once("includes/metadata-description-package-footer.php");
?>

<?php
if (pathinfo($_SERVER['SCRIPT_NAME'], PATHINFO_FILENAME) == "commit-transaction")
{
    // Do nothing
}
else
{
    // Validate template
    if (!$metadataTemplate->validate())
    {
        exitWithError("Failed to validate template");
    }
    
    // Return result
    $communicator->sendPlaintext($metadataTemplate->toXml());
    
    exitWithSuccess();
}
?>

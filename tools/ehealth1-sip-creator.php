<?php
include(__DIR__ . '/../ehealth1-sip.php');

$putputDir = './';
$informationPackageId = 'SAMPLE_ID';

// Define SIP
$outPath;
$sip = new Ehealth1Sip($informationPackageId);

// Add patients
// TODO: Create form script arguments
$patient1 = new Ehealth1SipPatient('PATIENT1_ID');
$patient1->addFile('./commit-transaction.php');
$patient1->addFile('./commit-transaction-ack.php');

// Output SIP
outPath;
if (!$sip->produceSip($outPath, $outputDir))
{
    echo "Failed to produce SIP";
    return 1;
}

echo "Wrote SIP to $outPath" . PHP_EOL;
?>

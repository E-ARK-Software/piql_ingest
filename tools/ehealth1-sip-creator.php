<?php
include(__DIR__ . '/../includes/ehealth1-sip.php');

$outputDir = './';
$informationPackageId = 'SAMPLE_ID';

// Define SIP
$outPath;
$sip = new Ehealth1Sip($informationPackageId);
$sip->addSubmissionAgreement('./SA.pdf');

// Add patients
// TODO: Create form script arguments
$patient1 = new Ehealth1SipPatient('PATIENT1_ID');
$patient1->addFile('commit-transaction.php', 'commit-transaction.php');
$patient1->addFile('commit-transaction-ack.php', 'commit-transaction-ack.php');
$sip->addPatient($patient1);

$patient2 = new Ehealth1SipPatient('PATIENT2_ID');
$patient2->addFile('includes/filetransfer', 'filetransfer');
$patient2->addFile('includes/filetransfer/file-transfer-base.php', 'filetransfer/file-transfer-base.php');
$patient2->addFile('includes/filetransfer/file-transfer-ssh.php', 'filetransfer/file-transfer-ssh.php');
$sip->addPatient($patient2);

// Output SIP
$outPath;
if (!$sip->produceSip($outPath, $outputDir))
{
    echo $sip->error() . PHP_EOL;
    echo "Failed to produce SIP" . PHP_EOL;
    return 1;
}

echo "Wrote SIP to $outPath" . PHP_EOL;
?>

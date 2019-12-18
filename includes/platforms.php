<?php
define("PLATFORM_CENTOS6", "CENTOS6");
define("PLATFORM_CENTOS7", "CENTOS7");
define("PLATFORM_WINDOWS7", "WINDOWS7");
define("PLATFORM_WINDOWS8", "WINDOWS8");
define("PLATFORM_WINDOWS10", "WINDOWS10");
define("PLATFORM_OSX", "OSX");
define("PLATFORM_UNIDENTIFIED", "UNIDENTIFIED");

class Platforms
{
    private $m_SupportedPlatforms = array();

    public function __construct()
    {
    }

    public function addSupported($platform)
    { 
        array_push($this->m_SupportedPlatforms, $platform);
    }

    public function isSupported($platform)
    {
        for ($i = 0; $i < count($this->m_SupportedPlatforms); $i++)
        {
            if ($this->m_SupportedPlatforms[$i] == $platform)
            {
                return true;
            }
        }

        return false;
    }

    public function identify(&$os, &$distribution)
    {
        $platform = PLATFORM_UNIDENTIFIED;
        $os = PHP_OS;
        $uname = php_uname();
        if (strtoupper(substr($os, 0, 3)) === 'LIN')
        {
            $command = "cat /etc/*-release";
            $ret = false;
            $output = system($command, $ret);
            $distribution = $output;
            if ($ret == 0 && strpos($output, "CentOS release 6") !== false)
            {
                $platform = PLATFORM_CENTOS6;
            }
            else if ($ret == 0 && strpos($output, "CentOS Linux release 7") !== false)
            {
                $platform = PLATFORM_CENTOS7;
            }
        }
        else if (strtoupper(substr($os, 0, 3)) === 'WIN')
        {
            $distribution = $uname;
            if (strpos($uname, "Windows 7") !== false)
            {
                $platform = PLATFORM_WINDOWS7;
            }
            else if (strpos($uname, "Windows 8") !== false)
            {
                $platform = PLATFORM_WINDOWS8;
            }
            else if (strpos($uname, "Windows 10") !== false)
            {
                $platform = PLATFORM_WINDOWS10;
            }
        }
        else if (strtoupper(substr($os, 0, 3)) === 'DAR')
        {
            $platform = PLATFORM_OSX;
        }

        return $platform;
    }
}
?>

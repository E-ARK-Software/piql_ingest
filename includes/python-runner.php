<?php
class PythonRunner
{
    private $m_LastError = "";

    public function __construct()
    {
    }

    public function executeScript($path, $arguments)
    {
        // Check that Python is installed
        if (!$this->pythonPath($pythonPath))
        {
            $this->setError("Python is not properly installed");
            return false;
        }

        // Compile command
        $command = "\"{$pythonPath}\" \"{$path}\"";
        foreach ($arguments as $argument)
        {
            $command .= " \"{$argument}\"";
        }

        // Write stderr to stdout
        $command .= " 2>&1";

        // Execute command
        $output = [];
        if (exec($command, $output, $returnCode) === false || $returnCode != 0)
        {
            $json = json_encode($output);
            $this->setError("Failed to execute command: '{$command}' returncode: '{$returnCode}' output: '{$json}'");
            return false;
        }
        $json = json_encode($output);
        logInfo("Executed command: '{$command}' returncode: '{$returnCode}' output: '{$json}'");

        return true;
    }

    public function error()
    {
        return $this->m_LastError;
    }

    private function setError($errorText)
    {
        $this->m_LastError = $errorText;
    }

    private function pythonPath(&$path)
    {
        $path = "";
        $checkPaths = [
            __DIR__ . "\\..\\thirdparty\\python\\python.exe",
            "python.exe",
            "python"
        ];
        foreach ($checkPaths as $checkPath)
        {
            $command = "\"{$checkPath}\" -V";
            if (system($command, $returnCode) !== false && $returnCode == 0)
            {
                $path = $checkPath;
                return true;
            }
        }

        return false;
    }
}
?>

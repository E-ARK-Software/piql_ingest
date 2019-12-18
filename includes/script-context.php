<?php
class ScriptContext
{
    private $m_Keys = array();
    private $m_Values = array();

    public function __construct()
    {
    }

    public function readFile($filePath)
    {
        // Read from file
        $context = file_get_contents($filePath);
        if (strlen($context) < 1)
        {
            return false;
        }

        return $this->readString($context);
    }

    /// \todo Use XML library to parse context - preserve CDATA tags which are part of value
    //        DScriptContext class must then output keys and values protected by CDATA
    public function readString($contextString)
    {
        $offset = 0;
        while (true)
        {
            $pos1 = strpos($contextString, "<parameter>", $offset);
            if ($pos1 !== false)
            {
                $pos2 = strpos($contextString, "</parameter>", $pos1);
                if ($pos2 === false)
                {
                    return false;
                }
                $offset = $pos2;
                $parameterStr = substr($contextString, $pos1, $pos2 - $pos1 + strlen("</parameter>"));

                $pos1 = strpos($parameterStr, "<key>");
                $pos2 = strpos($parameterStr, "</key>");
                if ($pos1 === false || $pos2 === false)
                {
                    $errorMessage = "Failed to find <key>";
                    return false;
                }
                array_push($this->m_Keys, substr($parameterStr, $pos1 + strlen("<key>"), $pos2 - $pos1 - strlen("<key>")));

                array_push($this->m_Values, array());
                $pos1 = strpos($parameterStr, "<values>");
                $pos2 = strpos($parameterStr, "</values>");
                if ($pos1 === false || $pos2 === false)
                {
                    return false;
                }
                $valuePos1 = $pos1;
                while (true)
                {
                    // Find next value
                    $valuePos1 = strpos($parameterStr, "<value>", $valuePos1);
                    $valuePos2 = strpos($parameterStr, "</value>", $valuePos1);
                    if ($valuePos1 === false || $valuePos2 === false || $valuePos1 > $pos2 || $valuePos2 > $pos2 || $valuePos1 > $valuePos2)
                    {
                        break;
                    }
                    
                    // Remove CDATA tag
                    $value = substr($parameterStr, $valuePos1 + strlen("<value>"), $valuePos2 - $valuePos1 - strlen("<value>"));
                    if (substr($value, 0, strlen("<![CDATA[")) == "<![CDATA[" && substr($value, strlen($value) - strlen("]]>")) == "]]>")
                    {
                        $value = substr($value, strlen("<![CDATA["), strlen($value) - strlen("<![CDATA[") - strlen("]]>"));
                    }
                    
                    // Store value
                    array_push($this->m_Values[count($this->m_Values) - 1], $value);
                    $valuePos1 = $valuePos2;
                }
            }
            else
            {
                break;
            }
        }

        if (count($this->m_Keys) != count($this->m_Values))
        {
            return false;
        }

        return true;
    }

    public function valueAsString(&$value, $key)
    {
        for ($i = 0; $i < count($this->m_Keys); $i++)
        {
            if ($this->m_Keys[$i] == $key)
            {
                if (count($this->m_Values[$i]) != 1)
                {
                    return false;
                }
                $value = $this->m_Values[$i][0];
                return true;
            }
        }

        return false;
    }

    public function valueAsArray(&$value, $key)
    {
        for ($i = 0; $i < count($this->m_Keys); $i++)
        {
            if ($this->m_Keys[$i] == $key)
            {
                $value = $this->m_Values[$i];
                return true;
            }
        }

        return false;
    }
}
?>

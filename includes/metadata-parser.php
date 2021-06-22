<?php
class MetadataParserKeyTemplate
{
    private $m_Key = "";
    private $m_Required = false;

    public function __construct($key, $required)
    {
        $this->m_Key = $key;
        $this->m_Required = $required;
    }

    public function key()
    {
        return $this->m_Key;
    }

    public function required()
    {
        return $this->m_Required;
    }
}

class MetadataParserItem
{
    private $m_Key = "";
    private $m_Value = "";
    private $m_Group = "";
    private $m_FileIndex = -1;

    public function __construct($key, $value, $group, $fileIndex)
    {
        $this->m_Key = $key;
        $this->m_Value = $value;
        $this->m_Group = $group;
        $this->m_FileIndex = $fileIndex;
    }

    public function key()
    {
        return $this->m_Key;   
    }

    public function value()
    {
        return $this->m_Value;
    }

    public function group()
    {
        return $this->m_Group;
    }
    
    public function fileIndex()
    {
        return $this->m_FileIndex;
    }
}

class MetadataParser
{
    private $m_Metadata;
    private $m_KeyTemplates = array();
    private $m_MetadataItems = array();

    public function __construct()
    {
    }
    
    public function itemCount()
    {
        return count($this->m_MetadataItems);
    }

    public function readFile($filePath, &$errorMessage)
    {
        $errorMessage = "";

        // Read from file
        $metadata = file_get_contents($filePath);
        if (strlen($metadata) < 1)
        {
            $errorMessage = "Failed to read file: $filePath";
            return false;    
        }

        return $this->readString($metadata, $errorMessage);
    }

    public function readString($metadataString, &$errorMessage)
    {
        $errorMessage = "";
        
        // Get file index
        $fileIndex = 0;
        if (count($this->m_MetadataItems) > 0)
        {
            $fileIndex = $this->m_MetadataItems[count($this->m_MetadataItems) - 1]->fileIndex() + 1;
        }

        // Load XML
        $this->m_Metadata = simplexml_load_string($metadataString);
        if ($this->m_Metadata === false)
        {
            $errorMessage = "Failed to load XML";
            return false;
        }

        // Parse XML
        foreach ($this->m_Metadata->children() as $group)
        {
            // Get group name
            $attributeName = "name";
            $groupName = (string)$group->attributes()->$attributeName;
            if (strlen($groupName) == 0)
            {
                $errorMessage = "Failed to get name of group";
                return false;
            }

            foreach ($group->children() as $item)
            {
                // Add to list
                array_push($this->m_MetadataItems, new MetadataParserItem($item->getName(), $item->__toString(), $groupName, $fileIndex));
            }
        }
        
        // Is metadata empty?
        if (count($this->m_MetadataItems) == 0)
        {
            return true;
        }

        // Check if keys are valid
        for ($i = 0; $i < count($this->m_MetadataItems); $i++)
        {
            if ($this->m_MetadataItems[$i]->fileIndex() == $fileIndex)
            {
                for ($j = 0; $j < count($this->m_KeyTemplates); $j++)
                {
                    if ($this->m_MetadataItems[$i]->key() == $this->m_KeyTemplates[$j]->key())
                    {
                        break;
                    }

                    if ($j == count($this->m_KeyTemplates) - 1)
                    {
                        $errorMessage = "Found invalid key: " . $this->m_MetadataItems[$i]->key();
                        return false;
                    }
                }
            }
        }

        // Check if all required keys are found
        for ($i = 0; $i < count($this->m_KeyTemplates); $i++)
        {
            if ($this->m_MetadataItems[$i]->fileIndex() == $fileIndex)
            {
                if ($this->m_KeyTemplates[$i]->required())
                {
                    for ($j = 0; $j < count($this->m_MetadataItems); $j++)
                    {
                        if ($this->m_KeyTemplates[$i]->key() == $this->m_MetadataItems[$j]->key())
                        {
                            break;
                        }

                        if ($j == count($this->m_MetadataItems) - 1)
                        {
                            $errorMessage = "Key not found: " . $this->m_KeyTemplates[$i]->key();
                            return false;
                        }
                    }
                }
            }
        }

        return true;
    }

    public function addKeyTemplate($keyTemplate)
    {
        $allowDuplicateKeys = true;
    
        if (!$allowDuplicateKeys)
        {
            // Check for duplicate keys
            for ($i = 0; $i < count($this->m_KeyTemplates); $i++)
            {
                if ($keyTemplate->key() == $this->m_KeyTemplates[$i]->key())
                {
                    return false;
                }
            }
        }

        array_push($this->m_KeyTemplates, $keyTemplate);
        return true;
    }

    public function getValue($key, $fileIndex, &$value)
    {
        for ($i = 0; $i < count($this->m_MetadataItems); $i++)
        {
            if ($this->m_MetadataItems[$i]->fileIndex() == $fileIndex && $this->m_MetadataItems[$i]->key() == $key)
            {
                $value = $this->m_MetadataItems[$i]->value();
                return true;
            }
        }

        return false;
    }

    public function getMetadata()
    {
        return $this->m_MetadataItems;
    }

    public function writeDublinCoreCsvToFile($metadataFileOut, $fileNameList, $outputOrder, $fondPosition)
    {
        // For Archivematica to process correctly, possitions of the first
        // four fields must be fixed: dc.filename, dc.title, dc.date, dc.description
        // For compound SIP only the first file will have a description (fond)
    
        // Get item list
        $metadataItems = $this->m_MetadataItems;
    
        // Validate metadata
        if (count($metadataItems) == 0)
        {
            return false;
        }
    
        // Validate filenames
        if (count($fileNameList) == 0 || count($metadataItems) % count($fileNameList) != 0)
        {
            return false;
        }
        
        // Order items
        if (count($outputOrder) != 0)
        {
            if (count($this->m_MetadataItems) % count($outputOrder) != 0)
            {
                return false;
            }
        
            $metadataItems = array();
            for ($i = 0; $i < count($this->m_MetadataItems) / count($outputOrder); $i++)
            {
                for ($j = 0; $j < count($outputOrder); $j++)
                {
                    array_push($metadataItems, $this->m_MetadataItems[$i*count($outputOrder)+$outputOrder[$j]]);
                }
            }
        }
        
        // Build content
        $title = "";
        $value = "";
        $lastIndex = -1;
        $startIndex = $metadataItems[0]->fileIndex();
        $currFileMetadataIndex = 0;
        for ($i = 0; $i < count($metadataItems); $i++)
        {
            // Write title
            if ($metadataItems[$i]->fileIndex() == $startIndex)
            {
                if ($i == 0)
                {
                    $title .= "filename";
                }
                $title .= ",";
                if (count($outputOrder) == 0)
                {
                    $title .= "dc." . $metadataItems[$i]->key();
                }
                else
                {
                    $title .= "dc." . $metadataItems[$i]->key();
                }
            }
        
            // Write value
            if ($metadataItems[$i]->fileIndex() == $lastIndex)
            {
                $currFileMetadataIndex++;
            }
            else
            {
                if ($i != 0)
                {
                    $value .= PHP_EOL;
                }
                $value .= "objects/" . $fileNameList[$metadataItems[$i]->fileIndex()];
                $currFileMetadataIndex = 0;
            }
            $value .=  ",";
            if ($currFileMetadataIndex == $fondPosition && $metadataItems[$i]->fileIndex() != $startIndex)
            {
                // Leave the fond blank for all files except for the first
            }
            else
            {
                $value .=  $metadataItems[$i]->value();
            }

            // Validate value
            if ($fondPosition >= 0 && $currFileMetadataIndex != 0 && $currFileMetadataIndex != $fondPosition)
            {
                // All values except filename, title and AtoM fond are shared and must be identical for all files
                if ($metadataItems[$currFileMetadataIndex]->value() != $metadataItems[$i]->value())
                {
                    return false;
                }
            }
            
            $lastIndex = $metadataItems[$i]->fileIndex();
        }

        // Write to file
        if (!file_put_contents($metadataFileOut, $title . PHP_EOL, FILE_APPEND | LOCK_EX) ||
            !file_put_contents($metadataFileOut, $value . PHP_EOL, FILE_APPEND | LOCK_EX))
        {
            return false;
        }

        // Validate written metadata
        $handle = fopen($metadataFileOut, "r");
        if ($handle)
        {
            $expectedLines = array($title . PHP_EOL);
            $expectedLinesValues = explode(PHP_EOL, $value);
            for ($i = 0; $i < count($expectedLinesValues); $i++)
            {
                array_push($expectedLines, $expectedLinesValues[$i] . PHP_EOL);
            }
            array_push($expectedLines, PHP_EOL);
            $lineCount = 0;
            while (($line = fgets($handle)) !== false)
            {
                if ($line != $expectedLines[$lineCount])
                {
                    return false;
                }
                $lineCount++;
            }

            if ($lineCount != count($fileNameList) + 1)
            {
                return false;
            }

            fclose($handle);
        }
        else
        {
            return false;
        }

        return true;
    }
}
?>

<?php
define("METADATA_TYPE_STRING", "STRING");
define("METADATA_TYPE_DROPDOWN", "DROPDOWN");
define("METADATA_TYPE_DATETIME", "DATETIME");

class MetadataTemplateItemGroup
{
    private $m_MetadataItems = array();
    private $m_Name = "";
    private $m_Description = "";
    
    public function __construct($name, $description)
    {
        $this->m_MetadataItems = array();
        $this->m_Name = $name;
        $this->m_Description = $description;
    }
    
    public function name()
    {
        return $this->m_Name;
    }
    
    public function description()
    {
        return $this->m_Description;
    }
    
    public function addItem($item)
    {
        array_push($this->m_MetadataItems, $item);
    }
    
    public function itemCount()
    {
        return count($this->m_MetadataItems);
    }
    
    public function getItem(&$item, $index)
    {
        if ($index >= count($this->m_MetadataItems) || $index < 0)
        {
            return false;
        }
    
        $item = $this->m_MetadataItems[$index];
        return true;
    }
    
    public function isValid()
    {
        for ($i = 0; $i < count($this->m_MetadataItems); $i++)
        {
            if (!$this->m_MetadataItems[$i]->isValid())
            {
                return false;
            }
        }
        
        return true;
    }
}

abstract class MetadataTemplateItemBase
{
    private $m_Name = "";
    private $m_Description = "";
    private $m_DefaultValue = "";
    private $m_Type = "";
    private $m_Data = "";
    private $m_ReadOnly = false;
    private $m_Visible = true;
    private $m_IsValid = true;
    private $m_Width = -1;
    private $m_FontSize = -1;
    private $m_LabelFontSize = -1;

    protected function __construct($name, $description, $defaultValue, $type)
    {
        $this->m_Name = $name;
        $this->m_Description = $description;
        $this->m_DefaultValue = $defaultValue;
        $this->m_Type = $type;
    }

    protected function addData($data)
    {
        $this->m_Data .= $data;
    }
    
    protected function setData($data)
    {
        $this->m_Data = $data;
    }
    
    protected function setValid($valid)
    {
        $this->m_IsValid = $valid;
    }

    public function setReadOnly($readOnly)
    {
        $this->m_ReadOnly = $readOnly;
    }
    
    public function setVisible($visible)
    {
        $this->m_Visible = $visible;
    }
    
    public function setWidth($pixels)
    {
        $this->m_Width = $pixels;
    }
    
    public function setFontSize($size)
    {
        $this->m_FontSize = $size;
    }
    
    public function setLabelFontSize($pixels)
    {
        $this->m_LabelFontSize = $pixels;
    }

    public function name()
    {
        return $this->m_Name;
    }

    public function description()
    {
        return $this->m_Description;
    }

    public function defaultValue()
    {
        return $this->m_DefaultValue;
    }

    public function type()
    {
        return $this->m_Type;
    }

    public function data()
    {
        return $this->m_Data;
    }

    public function readOnly()
    {
        return $this->m_ReadOnly;
    }
    
    public function visible()
    {
        return $this->m_Visible;
    }
    
    public function width()
    {
        return $this->m_Width;
    }
    
    public function fontSize()
    {
        return $this->m_FontSize;
    }
    
    public function labelFontSize()
    {
        return $this->m_LabelFontSize;
    }
    
    public function isValid()
    {
        // Run custom validation
        if (method_exists($this, 'isValidCustom'))
        {  
            if (!$this->isValidCustom())
            {
                return false;
            }
        }
        
        return $this->m_IsValid;
    }

    public function validateRegex($regex)
    {
        $this->addData("<validateregex>$regex</validateregex>");
    }
}

class MetadataTemplateItemString extends MetadataTemplateItemBase
{
    public function __construct($name, $description, $defaultValue)
    {
        parent::__construct($name, $description, $defaultValue, METADATA_TYPE_STRING);
    }
}

class MetadataTemplateItemDropdown extends MetadataTemplateItemBase
{
    private $m_OptionNames = array();
    private $m_OptionValues = array();

    public function __construct($name, $description, $defaultValue)
    {
        parent::__construct($name, $description, $defaultValue, METADATA_TYPE_DROPDOWN);
    }
    
    public function addOption($name, $value)
    {
        array_push($this->m_OptionNames, $name);
        array_push($this->m_OptionValues, $value);

        // Get existing data
        $data = parent::data();

        // Clear old options
        $posStart = strpos($data, "<options>");
        if ($posStart !== false)
        {
            $posEnd = strpos($data, "</options>");
            if ($posEnd !== false)
            {
                $data = substr($data, 0, $posStart) . substr($data, $posEnd + strlen("</options>"));
            }
        }

        // Set new options
        $data .= "<options>";
        for ($i = 0; $i < count($this->m_OptionNames); $i++)
        {
            $data .= "<option>";
            
            $data .= "<name><![CDATA[";
            $data .= $this->m_OptionNames[$i];
            $data .= "]]></name>";
            
            $data .= "<value><![CDATA[";
            $data .= $this->m_OptionValues[$i];
            $data .= "]]></value>";
            
            $data .= "</option>";
        }
        $data .= "</options>";
        
        parent::setData($data);
    }
    
    public function isValidCustom()
    {
        // Box cannot be empty
        if ($this->m_OptionNames == 0)
        {
            return false;
        }
        
        // Name/value mismatch?
        if (count($this->m_OptionNames) != count($this->m_OptionValues))
        {
            return false;
        }
        
        // Duplicate names are not allowed
        for ($i = 0; $i < count($this->m_OptionNames); $i++)
        {
            for ($j = $i + 1; $j < count($this->m_OptionNames); $j++)
            {
                if ($this->m_OptionNames[$i] == $this->m_OptionNames[$j])
                {
                    return false;
                }
            }
        }
        
        // Default value must exist in selection
        for ($i = 0; $i < count($this->m_OptionValues); $i++)
        {
            if ($this->m_OptionValues[$i] == parent::defaultValue())
            {
                break;
            }
            
            if ($i == count($this->m_OptionValues) - 1)
            {
                return false;
            }
        }
        
        return true;
    }
}

class MetadataTemplateItemDatetime extends MetadataTemplateItemBase
{
    public function __construct($name, $description, $defaultValue)
    {
        parent::__construct($name, $description, $defaultValue, METADATA_TYPE_DATETIME);
    }

    public function setFormat($format)
    {
        parent::addData("<format>$format</format>");
    }

    public function setMaxDate($year, $month, $day)
    {
        parent::addData("<maxdateyear>$year</maxdateyear>");
        parent::addData("<maxdatemonth>$month</maxdatemonth>");
        parent::addData("<maxdateday>$day</maxdateday>");
    }

    public function setMinDate($year, $month, $day)
    {
        parent::addData("<mindateyear>$year</mindateyear>");
        parent::addData("<mindatemonth>$month</mindatemonth>");
        parent::addData("<mindateday>$day</mindateday>");
    }

    public function setMaxTime($hour, $minute, $second)
    {
        parent::addData("<maxdatehour>$hour</maxdatehour>");
        parent::addData("<maxdateminute>$minute</maxdateminute>");
        parent::addData("<maxdatesecond>$second</maxdatesecond>");
    }

    public function setMinTime($hour, $minute, $second)
    {
        parent::addData("<mindatehour>$hour</mindatehour>");
        parent::addData("<mindateminute>$minute</mindateminute>");
        parent::addData("<mindatesecond>$second</mindatesecond>");
    }

    public function enableCalendar($enable)
    {
        if ($enable)
        {
            parent::addData("<enablecalendar>true</enablecalendar>");
        }
        else
        {
            parent::addData("<enablecalendar>false</enablecalendar>");
        }
    }
}

class MetadataTemplate
{
    private $m_MetadataItemGroups = array();

    public function __construct()
    {
        $this->m_MetadataItemGroups = array();
    }
    
    public function groupCount()
    {
        return count($this->m_MetadataItemGroups);
    }

    public function addGroup($group)
    {
        array_push($this->m_MetadataItemGroups, $group);
    }
    
    public function getGroup(&$group, $index)
    {
        if ($index >= count($this->m_MetadataItemGroups) || $index < 0)
        {
            return false;
        }
    
        $group = $this->m_MetadataItemGroups[$index];
        return true;
    }

    public function validate()
    {
        // Validate XML
        $xml = $this->toXml();
        $xmlObj = simplexml_load_string($xml);
        if ($xmlObj === false)
        {
            return false;
        }

        if (count($this->m_MetadataItemGroups) == 0)
        {
            return false;
        }
        
        for ($i = 0; $i < count($this->m_MetadataItemGroups); $i++)
        {
            if (!$this->m_MetadataItemGroups[$i]->isValid())
            {
                return false;
            }
        }

        return true;
    }

    public function toXml()
    {
        $output = "";
        $output .= "<metadata-form>";
        $output .= " <file-metadata>";
        for ($i = 0; $i < count($this->m_MetadataItemGroups); $i++)
        {
            $output .= "  <group name=\"" . $this->m_MetadataItemGroups[$i]->name() . "\" description=\"" . $this->m_MetadataItemGroups[$i]->description() . "\">";
        
            for ($j = 0; $j < $this->m_MetadataItemGroups[$i]->itemCount(); $j++)
            {
                $this->m_MetadataItemGroups[$i]->getItem($currItem, $j);

                $readOnlyAttr = "";
                if ($currItem->readOnly())
                {
                    $readOnlyAttr = " readonly=\"true\"";
                }
                
                $visibleAttr = "";
                if (!$currItem->visible())
                {
                    $visibleAttr = " visible=\"false\"";
                }
                
                $widthAttr = "";
                if ($currItem->width() >= 0)
                {
                    $widthAttr = " width=\"" . $currItem->width() . "\"";
                }
                
                $fontSizeAttr = "";
                if ($currItem->fontSize() >= 0)
                {
                    $fontSizeAttr = " fontsize=\"" . $currItem->fontSize() . "\"";
                }
                
                $labelFontSizeAttr = "";
                if ($currItem->labelFontSize() >= 0)
                {
                    $labelFontSizeAttr = " labelfontsize=\"" . $currItem->labelFontSize() . "\"";
                }

                $output .= "  <item$readOnlyAttr$visibleAttr$widthAttr$fontSizeAttr$labelFontSizeAttr>";
                $output .= "   <name><![CDATA[" . $currItem->name() . "]]></name>";
                $output .= "   <description><![CDATA[" . nl2br($currItem->description()) . "]]></description>";
                $output .= "   <type><![CDATA[" . $currItem->type() . "]]></type>";
                $output .= "   <default><![CDATA[" . $currItem->defaultValue() . "]]></default>";
                $output .= "   <data>" . $currItem->data() . "</data>";
                $output .= "  </item>";
            }
            
            $output .= "  </group>";
        }
        $output .= " </file-metadata>";
        $output .= "</metadata-form>";
        
        $dom = new DOMDocument;
        $dom->preserveWhiteSpace = false;
        $dom->formatOutput = true;
        $dom->loadXML($output);

        return $dom->saveXML();
    }
}
?>

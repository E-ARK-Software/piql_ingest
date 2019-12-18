<?php
class Configuration
{
    private $m_Keys = array();
    private $m_Values = array();

    public function __construct()
    {
        $this->m_Keys = array();
        $this->m_Values = array();
    }

    public function add($key, $value)
    {
        if ($this->exists($key))
        {
            for ($i = 0; $i < count($this->m_Keys); $i++)
            {
                if ($this->m_Keys[$i] == $key)
                {
                    array_push($this->m_Values[$i], $value);
                    return;
                }
            }
            
            exit(1);
        }

        array_push($this->m_Keys, $key);
        array_push($this->m_Values, array($value));
    }

    public function exists($key)
    {
        for ($i = 0; $i < count($this->m_Keys); $i++)
        {
            if ($this->m_Keys[$i] == $key)
            {
                return true;
            }
        }

        return false;
    }

    public function getValue($key)
    {
        for ($i = 0; $i < count($this->m_Keys); $i++)
        {
            if ($this->m_Keys[$i] == $key)
            {
                if (count($this->m_Values[$i]) == 1)
                {
                    return $this->m_Values[$i][0];
                }
                exit(1);
            }
        }
        exit(1);
    }
    
    public function getValues($key)
    {
        for ($i = 0; $i < count($this->m_Keys); $i++)
        {
            if ($this->m_Keys[$i] == $key)
            {
                return $this->m_Values[$i];
            }
        }
        exit(1);
    }
}
?>

<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:xs="http://www.w3.org/2001/XMLSchema"
  xmlns:tns="https://journal.prorenata.se/static/xml/journal_archive/1.0.3/patients"
  version="1.0">

    <xsl:output method="html" indent="yes" encoding="utf-8"/>

    <xsl:template match="/tns:JournalArchive">
        <html>
            <head>
                <title>Journalarkiv - <xsl:apply-templates mode="patient-display" select="tns:objects/tns:patients/tns:patient"/></title>
                <link href="../archive.css" rel="stylesheet" />
            </head>
            <body>
                <h1>Journalarkiv</h1>
                <h2><xsl:apply-templates mode="patient-display" select="tns:objects/tns:patients/tns:patient"/></h2>
                <p><a href="../manifest.xml">Tillbaka till översikten</a></p>

                <xsl:apply-templates mode="patient-card" select="tns:objects/tns:patients/tns:patient"/>
                <xsl:apply-templates select="tns:objects/tns:health_care_units/tns:healthcareunit" />
            </body>
        </html>
    </xsl:template>

    <xsl:template mode="patient-card" match="tns:patient">

        <h4>Identifieringsinformation</h4>
        <table class="patient-card">
            <tr><th>Arkiv-ID</th><td><xsl:value-of select="@id" /></td></tr>
            <tr><th>UUID</th><td><xsl:value-of select="tns:uuid" /></td></tr>
            <tr><th>Externt ID</th><td><xsl:value-of select="tns:external_id" /></td></tr>
        </table>

        <h4>Allmän information</h4>
        <table class="patient-card">
            <tr><th>Organisationsenhet</th><td><xsl:value-of select="tns:organization_unit_name" /></td></tr>
            <xsl:if test="tns:current_school_class_name/text()!=''">
                <tr><th>Skolklass</th><td><xsl:value-of select="tns:current_school_class_name" /></td></tr>
            </xsl:if>
            <tr><th>Förnamn</th><td><xsl:value-of select="tns:first_name" /></td></tr>
            <tr><th>Efternamn</th><td><xsl:value-of select="tns:last_name" /></td></tr>
            <tr><th>Personnummer</th><td><xsl:call-template name="personal_id"><xsl:with-param name="personal_id" select="tns:personal_id"/> </xsl:call-template></td></tr>
            <tr><th>Födelsedag</th><td><xsl:value-of select="tns:birth_date"/></td></tr>
            <tr><th>Kön</th><td>
                <xsl:choose>
                    <xsl:when test="tns:sex/text()='M'">Man</xsl:when>
                    <xsl:when test="tns:sex/text()='F'">Kvinna</xsl:when>
                    <xsl:otherwise>Okänd</xsl:otherwise>
                </xsl:choose>
            </td></tr>
            <tr><th>Skyddade personuppgifter</th><td>
                <xsl:choose>
                    <xsl:when test="tns:protected_identity/text()='true'">Ja</xsl:when>
                    <xsl:otherwise>Nej</xsl:otherwise>
                </xsl:choose>
            </td></tr>
            <tr><th>Avliden</th><td>
                <xsl:choose>
                    <xsl:when test="tns:deceased/text()='true'">Ja</xsl:when>
                    <xsl:otherwise>Nej</xsl:otherwise>
                </xsl:choose>
            </td></tr>
        </table>


        <h4>Bifogade filer</h4>
        <xsl:apply-templates select="/tns:JournalArchive/tns:objects/tns:patient_images/tns:patientimage"/>

        <xsl:variable name="patient_id" select="@id" />
        <xsl:apply-templates mode="contact-info" select="/tns:JournalArchive/tns:objects/tns:contact_info/tns:contactinfo[tns:object/@related_id=$patient_id]"/>

        <xsl:apply-templates mode="custodians" select="/tns:JournalArchive/tns:objects/tns:custodians/tns:custodian"/>

    </xsl:template>

    <xsl:template mode="custodians" match="tns:custodian">
         <xsl:variable name="custodian_id" select="@id" />
        <h4>Vårdnadshavare</h4>
        <h5><xsl:value-of select="tns:first_name" /><xsl:text> </xsl:text><xsl:value-of select="tns:last_name" /></h5>
        <h5><xsl:value-of select="tns:personal_id" /></h5>
        <p><xsl:value-of select="tns:note"/></p>


        <xsl:apply-templates mode="contact-info" select="/tns:JournalArchive/tns:objects/tns:contact_info/tns:contactinfo[tns:object/@related_id=$custodian_id]"/>
    </xsl:template>

    <xsl:template mode="contact-info" match="tns:contactinfo">
        <h4>Kontaktinformation</h4>

        <xsl:for-each select="tns:addresses/tns:address">
            <h4><xsl:value-of select="tns:description"/></h4>
            <div><xsl:value-of select="tns:address1" /></div>
            <div><xsl:value-of select="tns:address2" /></div>
            <div><xsl:value-of select="tns:zip" /><xsl:text> </xsl:text><xsl:value-of select="tns:city" /></div>
            <div><xsl:value-of select="tns:region" /><xsl:text> </xsl:text><xsl:value-of select="tns:country" /></div>
        </xsl:for-each>

        <table style="margin-top:1em;">
        <xsl:for-each select="tns:phone_numbers/tns:phonenumber[tns:number!='']">
            <tr><th><xsl:value-of select="tns:description"/></th><td><xsl:value-of select="tns:number" /></td></tr>
        </xsl:for-each>
        </table>

        <table style="margin-top:1em;">
        <xsl:for-each select="tns:e_mails/tns:email[tns:mail!='']">
            <tr><th><xsl:value-of select="tns:description"/></th><td><xsl:value-of select="tns:mail" /></td></tr>
        </xsl:for-each>
        </table>
    </xsl:template>

    <xsl:template match="tns:healthcareunit">
        <section>
            <h2><xsl:value-of select="tns:journal_label" /></h2>
            <p>Journal för <xsl:value-of select="tns:journal_label" /> på <xsl:value-of select="tns:unit_name" /></p>
            <xsl:variable name="hcu_id" select="@id" />
            <xsl:apply-templates select="/tns:JournalArchive/tns:objects/tns:admissions/tns:admission[tns:health_care_unit/@related_id=$hcu_id]" />
            <div class="watches">
            <xsl:apply-templates select="/tns:JournalArchive/tns:objects/tns:watches/tns:patientwatch[tns:health_care_unit/@related_id=$hcu_id]" />
            </div>
            <div class="tasks">
            <xsl:apply-templates select="/tns:JournalArchive/tns:objects/tns:tasks/tns:task[tns:health_care_unit/@related_id=$hcu_id]" />
            </div>
            <xsl:apply-templates select="/tns:JournalArchive/tns:objects/tns:entries/tns:journalentry[tns:health_care_unit/@related_id=$hcu_id]" />
        </section>
    </xsl:template>

    <xsl:template match="tns:admission">
            <div>
                <a>
                    <xsl:attribute name="href"><xsl:value-of select="tns:pdf_archive_path"/></xsl:attribute>
                    PDF-utskrift
                </a>
            </div>
        <xsl:if test="growth_curve_archive_path/text()!=''">
            <div>
                <a>
                    <xsl:attribute name="href"><xsl:value-of select="tns:growth_curve_archive_path"/></xsl:attribute>
                    Tillväxtkurva
                </a>
            </div>
        </xsl:if>
        <xsl:if test="tns:vaccination_card_archive_path/text()!=''">
            <div>
                <a>
                    <xsl:attribute name="href"><xsl:value-of select="tns:vaccination_card_archive_path"/></xsl:attribute>
                    Vaccinationskort
                </a>
            </div>
        </xsl:if>
    </xsl:template>

    <xsl:template match="tns:patientwatch">
        <div>
            <xsl:attribute name="class">patientwatch <xsl:value-of select="tns:level"/></xsl:attribute>
            <h3><xsl:value-of select="tns:reason"/></h3>
            <p><xsl:value-of select="tns:note" disable-output-escaping="yes"/></p>
        </div>
    </xsl:template>

    <xsl:template match="tns:task">
        <div class="task">
            <h3><xsl:value-of select="tns:header"/></h3>
            <table>
                <tr><th>Ansvarig</th><td><xsl:value-of select="tns:user_name"/></td></tr>
                <tr><th>Skapad av</th><td><xsl:value-of select="tns:created_by_name"/></td></tr>
                <tr><th>Skapad</th><td><xsl:value-of select="tns:created_date_time"/></td></tr>
                <xsl:if test="string-length(due_date_time)!=0">
                    <tr><th>Avklaras senast</th><td><xsl:value-of select="tns:due_date_time"/></td></tr>
                </xsl:if>
                <tr><th>Avklarad</th><td>
                    <xsl:choose>
                        <xsl:when test="string-length(done_date_time)=0">
                            Nej
                        </xsl:when>
                        <xsl:otherwise>
                            Av <xsl:value-of select="tns:done_by_name"/>, <xsl:value-of select="tns:done_date_time"/>
                        </xsl:otherwise>
                    </xsl:choose>
                </td></tr>
            </table>
            <p><xsl:value-of select="tns:description" disable-output-escaping="yes" /></p>
        </div>
    </xsl:template>


    <xsl:template match="tns:journalentry">
        <xsl:variable name="entry_id" select="@id" />
        <div class="journalentry" id="$entry_id">
            <h2><xsl:value-of select="tns:title"/></h2>
        <a><xsl:attribute name="name"><xsl:value-of select="$entry_id" /></xsl:attribute></a>
        <xsl:apply-templates mode="attached_to" select="/tns:JournalArchive/tns:objects/tns:attached_entries/tns:attachedjournalentry[tns:attached_entry/@related_id=$entry_id]"/>
        <xsl:value-of select="tns:html" disable-output-escaping="yes"/>
        <h4>Kopplade anteckningar</h4>
        <xsl:apply-templates mode="attachments" select="/tns:JournalArchive/tns:objects/tns:attached_entries/tns:attachedjournalentry[tns:main_entry/@related_id=$entry_id]"/>
        <h4>Bifogade dokument</h4>
        <xsl:apply-templates select="tns:attachments/tns:journalentrys3attachment"/>
        </div>
    </xsl:template>

    <xsl:template mode="attached_to" match="tns:attachedjournalentry">
        Kopplad till <a><xsl:attribute name="href">#<xsl:value-of select="tns:main_entry/@related_id"/></xsl:attribute><xsl:value-of select="tns:main_entry_title"/> (<xsl:value-of select="tns:main_entry_entry_date_time"/>)</a>
    </xsl:template>
    <xsl:template mode="attachments" match="tns:attachedjournalentry">
        <xsl:if test="attached_entry/@related_id!=''">
         <div>
            <a><xsl:attribute name="href">#<xsl:value-of select="tns:attached_entry/@related_id"/></xsl:attribute><xsl:value-of select="tns:attached_entry_title"/> (<xsl:value-of select="tns:attached_entry_entry_date_time"/>)</a>
            </div>
        </xsl:if>
    </xsl:template>

    <xsl:template match="tns:journalentrys3attachment">
        <div>
        <a>
            <xsl:attribute name="href"><xsl:value-of select="tns:archive_path"/></xsl:attribute>
            <xsl:value-of select="tns:filename"/>
        </a>
        <xsl:if test="archive_pdfa_path">
            &#160;
            <a>
                <xsl:attribute name="href"><xsl:value-of select="tns:archive_pdfa_path"/></xsl:attribute>(PDF/A)
            </a>
        </xsl:if>
        </div>
    </xsl:template>


    <xsl:template match="tns:patientimage">
        <div>
        <a>
            <xsl:attribute name="href"><xsl:value-of select="tns:archive_path"/></xsl:attribute>
            <xsl:value-of select="tns:filename"/>
        </a>
        <xsl:if test="tns:archive_pdfa_path">
            &#160;
            <a>
                <xsl:attribute name="href"><xsl:value-of select="tns:archive_pdfa_path"/></xsl:attribute>(PDF/A)
            </a>
        </xsl:if>
        </div>
    </xsl:template>


    <!--
    This code is also in manifest.xslt
    Any changes here should be reflected there as well.
    -->
    <xsl:template mode="patient-display" match="tns:patient">
        <xsl:value-of select="tns:first_name"/>
        <xsl:text> </xsl:text>
        <xsl:value-of select="tns:last_name"/><xsl:text> (</xsl:text><xsl:call-template name="personal_id"><xsl:with-param name="personal_id" select="tns:personal_id"/> </xsl:call-template>)
    </xsl:template>

    <xsl:template name="personal_id">
        <xsl:param name="personal_id" />
        <xsl:value-of select="substring($personal_id,1,2)"/><xsl:text> </xsl:text><xsl:value-of select="substring($personal_id,3,6)"/>-<xsl:value-of select="substring($personal_id,9)"/>
    </xsl:template>


</xsl:stylesheet>

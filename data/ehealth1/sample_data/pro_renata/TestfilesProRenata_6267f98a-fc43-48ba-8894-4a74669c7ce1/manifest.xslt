<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:tns="https://journal.prorenata.se/static/xml/journal_archive/1.0.3/manifest"
  version="1.0">

    <xsl:output method="html" indent="yes" encoding="utf-8"/>

    <xsl:template match="/tns:JournalArchiveManifest">
        <html>
            <head>
                <title>Journalarkiv</title>
            </head>
            <body>
                <h1>Journalarkiv</h1>
                <h2><xsl:value-of select="tns:organization/text()"/></h2>
                <div>
                    <dl>
                        <dt>Skapad av:</dt><dd><xsl:value-of select="tns:created_by/tns:title/text()"/>&#160;<xsl:value-of select="tns:created_by/tns:first_name/text()"/>&#160;<xsl:value-of select="tns:created_by/tns:last_name/text()"/></dd>
                        <dt>Skapelsetid:</dt><dd><xsl:value-of select="tns:created_date_time" /></dd>
                        <dt>Exkludera sekretessmarkerade anteckningar:</dt><dd><xsl:value-of select="tns:exclude_confidential" /></dd>
                        <dt>Exkludera tomma journaler:</dt><dd><xsl:value-of select="tns:exclude_empty_journals" /></dd>
                    </dl>
                </div>
                <h3>Inkluderade dokumentationsmappar</h3>
                <ul>
                    <xsl:apply-templates select="tns:health_care_unit_types/tns:health_care_unit_type" />
                </ul>
                <h3>Kriterier</h3>
                <xsl:apply-templates select="tns:criteria/tns:criterion"/>
                <h3>Fel</h3>
                <xsl:apply-templates select="tns:errors"/>
                <h3>Varningar</h3>
                <xsl:apply-templates select="tns:warnings"/>
                <h3>Inkluderade patienter</h3>
                <table>
                    <thead>
                        <th>Personnummer</th>
                        <th>Förnamn</th>
                        <th>Efternamn</th>
                        <th>Organisationsenhet</th>
                    </thead>
                    <tbody>
                    <xsl:apply-templates select="tns:patients/tns:patient"/>
                    </tbody>
                </table>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="tns:criterion[tns:type_id/text()='by_id']">
        <div class="criterion">
            Någon av följande patienter
            <ul>
                <xsl:for-each select="tns:options/tns:patient">
                    <li><xsl:apply-templates mode="patient-display" select="."/></li>
                </xsl:for-each>
            </ul>
        </div>
    </xsl:template>

    <xsl:template match="tns:criterion[tns:type_id/text()='by_org_unit']">
        <div class="tns:criterion">
            Någon av följande organisationsenheter
            <ul>
                <xsl:for-each select="tns:options/tns:organization_unit">
                    <li><xsl:value-of select="tns:name" /></li>
                </xsl:for-each>
            </ul>
        </div>
    </xsl:template>

    <xsl:template match="tns:criterion[type_id/text()='by_birth_year']">
        <div class="tns:criterion">
            Född
            <xsl:choose>
                <xsl:when test="tns:options/tns:comparison/text() = 'lt'">
                    före <xsl:value-of select="tns:options/tns:birth_year" />-01-01
                </xsl:when>
                <xsl:when test="tns:options/tns:comparison/text() = 'exact'">
                    under året <xsl:value-of select="tns:options/tns:birth_year" />-01-01
                </xsl:when>
                <xsl:when test="tns:options/tns:comparison/text() = 'lt'">
                    efter <xsl:value-of select="tns:options/tns:birth_year" />-12-31
                </xsl:when>
            </xsl:choose>
        </div>
    </xsl:template>

    <xsl:template match="tns:criterion">
        <p>
            OKÄNT KRITERIUM : <xsl:value-of select="tns:options" />
        </p>
    </xsl:template>

    <xsl:template match="tns:health_care_unit_type">
        <li><xsl:value-of select="tns:label" /></li>
    </xsl:template>

    <xsl:template match="tns:patient">
        <tr>
            <td><xsl:value-of select="tns:personal_id" /></td>
            <td><xsl:value-of select="tns:first_name" /></td>
            <td><xsl:value-of select="tns:last_name" /></td>
            <td><xsl:value-of select="tns:organization_unit" /></td>
            <td><a>
                <xsl:attribute name="href">
                    <xsl:value-of select="tns:archive_path"/>
                </xsl:attribute>
                Gå till journal
            </a></td>
        </tr>
    </xsl:template>


    <xsl:template match="tns:errors|tns:warnings">
        <ul>
            <xsl:apply-templates select="tns:error|tns:warning"/>
        </ul>
    </xsl:template>

    <xsl:template match="tns:error|tns:warning">
        <li><xsl:value-of select="text()" /></li>
    </xsl:template>

    <!--
    This code is also in patient.xslt
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

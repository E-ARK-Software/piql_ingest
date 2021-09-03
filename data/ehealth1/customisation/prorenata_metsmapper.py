
import os.path
import sys
from sys import argv
import shutil
from xml.etree import ElementTree


def get_file_id(file):
    return file.split(" ")[0]


def transform(directory):
    """
    Generates the output folder structure for the input package
    :param directory:
    :return:
    """
    print("Starting transform")
    root_directory = os.path.basename(os.path.normpath(directory))
    print("Root dir:", root_directory)
    package_id = os.path.normpath(os.path.basename(directory))

    # OVERWRITE
    if os.path.isdir(os.path.join(output_directory, package_id)):
        shutil.rmtree(os.path.join(output_directory, package_id))

    for file in os.listdir(directory):
        patient_id = get_file_id(file)
        if os.path.isdir(os.path.join(directory,file)):
            os.makedirs(os.path.join(output_directory, package_id, "patientrecord_"+patient_id, "case", "document"))

    # TODO Overwrite Y/N?


def move_files(directory):
    package_id = os.path.normpath(os.path.basename(directory))
    for file in os.listdir(directory):  # For each file in given directory
        if os.path.isdir(os.path.join(directory, file)):
            patient_id = file.split(" ")[0]
            if os.path.isdir(os.path.join(output_directory, package_id)):   # If the directory exits in the output folder
                for nextfile in os.listdir(os.path.join(directory, file)):  # For each file in patient folder
                    if os.path.isdir(os.path.join(directory, file, nextfile)):  # Get medicinsk-elevhalsa folder
                        if nextfile == "medicinsk-elevhalsa":
                            for datafile in os.listdir(os.path.join(directory, file, nextfile)):    # Copy each document to output folder
                                # print("DATA: "+datafile)
                                shutil.copy(os.path.join(directory, file, nextfile, datafile), os.path.join(output_directory, package_id, "patientrecord_"+patient_id, "case", "document"))
                        else:
                            print("Error with data file - medicinsk-elevhalsa - move_files()")
                    else:
                        shutil.copy(os.path.join(directory, file, nextfile), os.path.join(output_directory, package_id, "patientrecord_"+patient_id,))
            elif os.path.isfile(os.path.join(directory, file)):
                print("An error occurred - move-files()")
                sys.exit(1)
        elif os.path.isfile(os.path.join(directory, file)):
            print(file)
            if file == "manifest.xml":
                shutil.copy(os.path.join(directory, file), os.path.join(output_directory, package_id, "Patients.xml"))


def create_xml(directory):
    package_id = os.path.normpath(os.path.basename(directory))
    xml_tree = None
    for file in os.listdir(directory):
        if os.path.isdir(os.path.join(directory, file)):
            for nextfile in os.listdir(os.path.join(directory, file)):
                if os.path.splitext(nextfile)[1] == ".xml":
                    if xml_tree is None:
                        ElementTree.register_namespace("xsi", "http://www.w3.org/2001/XMLSchema-instance")
                        #ElementTree.register_namespace("noNamespaceSchemaLocation", "../patient.xsd")
                        ElementTree.register_namespace("", "https://journal.prorenata.se/static/xml/journal_archive/1.0.3/patients")
                        xml_tree = ElementTree.parse(os.path.join(directory, file, nextfile)).getroot()
                        print("LOL "+nextfile)
                        print(xml_tree)
                        print(xml_tree.findtext("objects"))
                        for element in xml_tree.iter():
                            if 'objects' in str(element):
                                print(element.keys())
                                print(element.items())
                                print(element.text)
                                #print(element)
                        #xml_tree.set("version", "1.0")
                    else:
                        additional_xml = ElementTree.parse(os.path.join(directory, file, nextfile)).getroot()
                        # print(additional_xml.attrib)
                        for element in additional_xml.iter():
                            if 'objects' in element.tag:
                                xml_tree.append(element)
    ElementTree.ElementTree(xml_tree).write(os.path.join(output_directory, package_id, "patients.xml"), encoding="UTF-8")


if __name__ == '__main__':
    if len(argv) > 1 and os.path.isdir(argv[1]):
        if len(argv) > 2 and os.path.isdir(argv[2]):
            output_directory = argv[2]
            transform(argv[1])
            move_files(argv[1])
            # create_xml(argv[1])
        else:
            print("Output directory error")
    else:
        print("Input directory error")

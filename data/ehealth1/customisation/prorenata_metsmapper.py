import os.path
import sys
from sys import argv
import shutil


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
        patient_id = file.split(" ")[0]
        if os.path.isdir(os.path.join(directory, file)):
            os.makedirs(os.path.join(output_directory, package_id, "patientrecord_" + patient_id, "case", "document"))

    # TODO Overwrite Y/N?


def move_files(directory):
    package_id = os.path.normpath(os.path.basename(directory))
    for file in os.listdir(directory):
        if os.path.isdir(os.path.join(directory, file)):
            patient_id = file.split(" ")[0]
            if os.path.isdir(os.path.join(output_directory, package_id)):  # Ensure directory exits in output folder
                for nextfile in os.listdir(os.path.join(directory, file)):  # For each file in patient folder
                    if os.path.isdir(os.path.join(directory, file, nextfile)):  # Get medicinsk-elevhalsa folder
                        if nextfile == "medicinsk-elevhalsa":
                            for datafile in os.listdir(
                                    os.path.join(directory, file, nextfile)):  # Copy each document to output folder
                                shutil.copy(os.path.join(directory, file, nextfile, datafile),
                                            os.path.join(output_directory, package_id, "patientrecord_" + patient_id,
                                                         "case", "document"))
                        else:
                            print("Error with data file - medicinsk-elevhalsa - move_files()")
                            sys.exit(1)
                    elif os.path.splitext(nextfile)[1] == ".xml" and nextfile.split(" ")[0] == patient_id:
                        print(nextfile)
                        clinicalinfofile = nextfile.split(",")[0] + "_condition.xml"
                        shutil.copy(os.path.join(directory, file, nextfile),
                                    os.path.join(output_directory, package_id, "patientrecord_" + patient_id,
                                                 clinicalinfofile))
                    else:
                        print("Found unexpected file: " + nextfile)
                        sys.exit(1)
            elif os.path.isfile(os.path.join(directory, file)):
                print("An error occurred - move-files()")
                sys.exit(1)
        elif os.path.isfile(os.path.join(directory, file)):
            if file == "manifest.xml":
                shutil.copy(os.path.join(directory, file), os.path.join(output_directory, package_id, "Patients.xml"))


if __name__ == '__main__':
    if len(argv) > 1 and os.path.isdir(argv[1]):
        if len(argv) > 2 and os.path.isdir(argv[2]):
            output_directory = argv[2]
            transform(argv[1])
            move_files(argv[1])
        else:
            print("Output directory error")
            sys.exit(1)
    else:
        print("Input directory error")
        sys.exit(1)

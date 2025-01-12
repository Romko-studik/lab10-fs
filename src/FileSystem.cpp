
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cstring>
#include <iomanip>

#pragma pack(push, 1)
struct fat_boot_sector
{
    char ignore1[3];
    char oem_name[8];
    unsigned short bytes_per_sector;
    unsigned char sectors_per_cluster;
    unsigned short reserved_sector_count;
    unsigned char table_count;
    unsigned short root_entry_count;
    unsigned short total_sectors_16;
    unsigned char media_type;
    unsigned short table_size_16;
    unsigned short sectors_per_track;
    unsigned short head_side_count;
    unsigned int hidden_sector_count;
    unsigned int total_sectors_32;
    unsigned char bios_drive_num;
    unsigned char ignore2;
    unsigned char ext_boot_signature;
    unsigned int volume_id;
    char volume_label[11];
    char fs_type[8];
    char boot_code[448];
    unsigned short boot_sector_signature;
};
#pragma pack(pop)

struct root_directory_entry
{
    char file_name[8];
    char file_extension[3];
    unsigned char attributes;
    char ignore1[8];
    unsigned short last_write_time;
    unsigned short last_write_date;
    unsigned short first_logical_cluster;
    unsigned int file_size;
};

struct root_directory
{
    root_directory_entry entries[224];
};

struct file_attributes
{
    unsigned char read_only : 1;
    unsigned char hidden : 1;
    unsigned char system : 1;
    unsigned char volume_id : 1;
    unsigned char directory : 1;
    unsigned char archive : 1;
    unsigned char device : 1;
    unsigned char unused : 1;
};

class FileSystem
{
public:
    FileSystem(std::string path) : path(path)
    {
        file.open(path, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open file system image at path: " << path << std::endl;
            exit(1);
        }
        find_mbr_offset();
        readBootSector();
        readRootSector();
    }
    void read()
    {
        std::cout << "Reading file system at path: " << path << std::endl;
    }
    void printBootSector()
    {
        std::cout << "Sector size: " << boot_sector.bytes_per_sector << std::endl;
        std::cout << "Cluster number: " << (int)boot_sector.sectors_per_cluster << std::endl;
        std::cout << "Fat count: " << (int)boot_sector.table_count << std::endl;
        std::cout << "Fat size: " << boot_sector.table_size_16 << std::endl;
        std::cout << "Entry count: " << boot_sector.root_entry_count << std::endl;
        std::cout << "Root size: " << boot_sector.root_entry_count * 32 / boot_sector.bytes_per_sector << std::endl;
        std::cout << "Reserved sectors: " << boot_sector.reserved_sector_count << std::endl;
        if (boot_sector.boot_sector_signature == 0xaa55)
        {
            std::cout << "Boot sector signature is valid" << std::endl;
        }
        else
        {
            std::cout << "Boot sector signature is invalid (expected 0xAA55)" << std::endl;
        }
    }

    void printBootSectorAll()
    {
        std::string oem_name(boot_sector.oem_name, 8);
        std::string volume_label(boot_sector.volume_label, 11);
        std::string fs_type(boot_sector.fs_type, 8);

        // Trim trailing spaces
        oem_name = oem_name.substr(0, oem_name.find_last_not_of(' ') + 1);
        volume_label = volume_label.substr(0, volume_label.find_last_not_of(' ') + 1);
        fs_type = fs_type.substr(0, fs_type.find_last_not_of(' ') + 1);

        std::cout << "OEM Name: " << oem_name << std::endl;
        std::cout << "Bytes per sector: " << boot_sector.bytes_per_sector << std::endl;
        std::cout << "Sectors per cluster: " << (int)boot_sector.sectors_per_cluster << std::endl;
        std::cout << "Reserved sector count: " << boot_sector.reserved_sector_count << std::endl;
        std::cout << "Table count: " << (int)boot_sector.table_count << std::endl;
        std::cout << "Root entry count: " << boot_sector.root_entry_count << std::endl;
        std::cout << "Total sectors 16: " << boot_sector.total_sectors_16 << std::endl;
        std::cout << "Media type: 0x" << std::hex << (int)boot_sector.media_type << std::dec << std::endl;
        std::cout << "Table size 16: " << boot_sector.table_size_16 << std::endl;
        std::cout << "Sectors per track: " << boot_sector.sectors_per_track << std::endl;
        std::cout << "Head side count: " << boot_sector.head_side_count << std::endl;
        std::cout << "Hidden sector count: " << boot_sector.hidden_sector_count << std::endl;
        std::cout << "Total sectors 32: " << boot_sector.total_sectors_32 << std::endl;
        std::cout << "BIOS drive number: 0x" << std::hex << (int)boot_sector.bios_drive_num << std::dec << std::endl;
        std::cout << "Extended boot signature: 0x" << std::hex << (int)boot_sector.ext_boot_signature << std::dec << std::endl;
        std::cout << "Volume id: " << boot_sector.volume_id << std::endl;
        std::cout << "Volume label: " << volume_label << std::endl;
        std::cout << "FS type: " << fs_type << std::endl;
        std::cout << "Boot sector signature: 0x" << std::hex << boot_sector.boot_sector_signature << std::dec << std::endl;

        if (boot_sector.boot_sector_signature == 0xAA55)
        {
            std::cout << "Boot sector signature is valid" << std::endl;
        }
        else
        {
            std::cout << "Boot sector signature is invalid (expected 0xAA55)" << std::endl;
        }
    }

    void printAttributes(unsigned char attributes)
    {
        file_attributes attr = readAttributes(attributes);
        if(attr.read_only)
        {
            std::cout << "Read only" << std::endl;
        }
        if(attr.hidden)
        {
            std::cout << "Hidden" << std::endl;
        }
        if(attr.system)
        {
            std::cout << "System" << std::endl;
        }
        if(attr.volume_id)
        {
            std::cout << "Volume ID" << std::endl;
        }
        if(attr.directory)
        {
            std::cout << "Directory" << std::endl;
        }
        if(attr.archive)
        {
            std::cout << "Archive" << std::endl;
        }
        if(attr.device)
        {
            std::cout << "Device" << std::endl;
        }
        if(attr.unused)
        {
            std::cout << "Unused" << std::endl;
        }
        
    }
    void printRootSector()
    {
        for (int i = 0; i < 224; i++)
        {
            root_directory_entry entry = root_dir.entries[i];
            if (entry.file_name[0] == static_cast<char>(0xE5))
            {
                break;
            }
            if (entry.file_name[0] == static_cast<char>(0x00))
            {
                continue;
            }
            std::string file_name(entry.file_name, 8);
            std::string file_extension(entry.file_extension, 3);
            std::cout << "File: " << file_name << "." << file_extension << std::endl;
            if (entry.attributes & 0x10)
            {
                std::cout << "Directory" << std::endl;
            }
            else
            {
                std::cout << "Size: " << entry.file_size << " bytes" << std::endl;
            }
            std::cout << "Last write date and time: " << decodeDateTime(entry.last_write_date, entry.last_write_time) << std::endl;
            printAttributes(entry.attributes);
            std::cout << "First logical cluster: " << entry.first_logical_cluster << std::endl;
            std::cout << "First sector: " << (entry.first_logical_cluster - 2) * boot_sector.sectors_per_cluster + boot_sector.reserved_sector_count + boot_sector.table_count * boot_sector.table_size_16 << std::endl;
            std::cout << std::endl;
        }
    }

private:
    std::ifstream file;
    std::string path;
    fat_boot_sector boot_sector;
    std::vector<uint8_t> buffer;
    root_directory root_dir;
    int f_offset = 0;

    void readBootSector()
    {
        file.seekg(f_offset);
        file.read(reinterpret_cast<char *>(&boot_sector), sizeof(fat_boot_sector));
    }
    std::vector<uint8_t> readBytes(int offset, int count)
    {
        std::vector<uint8_t> bytes;
        file.seekg(offset);
        file.read(reinterpret_cast<char *>(bytes.data()), count);
        return bytes;
    }
    void readRootSector()
    {
        file.seekg(512 * (boot_sector.reserved_sector_count + boot_sector.table_count * boot_sector.table_size_16) + f_offset);
        file.read(reinterpret_cast<char *>(&root_dir), sizeof(root_directory));
    }
    void readFiles()
    {
        file.seekg(512);
    }
    file_attributes readAttributes(unsigned char attributes)
    {
        file_attributes attr;
        attr.read_only = attributes & 0x01;
        attr.hidden = attributes & 0x02;
        attr.system = attributes & 0x04;
        attr.volume_id = attributes & 0x08;
        attr.directory = attributes & 0x10;
        attr.archive = attributes & 0x20;
        attr.device = attributes & 0x40;
        attr.unused = attributes & 0x80;
        return attr;
    }
    std::string decodeDateTime(uint16_t date, uint16_t time)
    {

        unsigned int hours = (time >> 11) & 0x1F;
        unsigned int minutes = (time >> 5) & 0x3F;
        unsigned int seconds = ((time & 0x1F) * 2);

        unsigned int day = date & 0x1F;
        unsigned int month = (date >> 5) & 0x0F;
        unsigned int year_offset = (date >> 9);
        unsigned int year = 1980 + year_offset;

        std::ostringstream oss;
        oss << std::setfill('0')
            << std::setw(4) << year << "-"
            << std::setw(2) << month << "-"
            << std::setw(2) << day << " "
            << std::setw(2) << hours << ":"
            << std::setw(2) << minutes << ":"
            << std::setw(2) << seconds;

        return oss.str();
    }
    void find_mbr_offset()
    {
        char buffer[512];
        file.seekg(0);          // Go to the start of the file
        file.read(buffer, 512); // Read the first 512 bytes

        // Check for the signature at 510–511
        if (buffer[510] == static_cast<char>(0x55) && buffer[511] == static_cast<char>(0xAA))
        {
            // Attempt to validate the MBR
            unsigned int partition_one =
                static_cast<unsigned char>(buffer[0x1BE + 8]) |
                (static_cast<unsigned char>(buffer[0x1BE + 9]) << 8) |
                (static_cast<unsigned char>(buffer[0x1BE + 10]) << 16) |
                (static_cast<unsigned char>(buffer[0x1BE + 11]) << 24);

            if (partition_one > 0 && partition_one < 0xFFFFFFF) // Validate LBA range
            {
                f_offset = partition_one * 512;
                std::cout << "Valid MBR detected." << std::endl;
                std::cout << "Partition 1 starting LBA: " << partition_one << std::endl;
                std::cout << "FAT16 Boot Sector offset: " << f_offset << " bytes" << std::endl;
            }
            else
            {
                f_offset = 0;
                std::cout << "No valid partition table. Assuming FAT16 Boot Sector at offset 0." << std::endl;
            }
        }
        else
        {
            std::cout << "No valid MBR signature found." << std::endl;
        }
    }
};
#include "hako_asset_runner.h"
#include "hako.hpp"
#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

struct PduReader {
    std::string type;
    std::string org_name;
    std::string name;
    int channel_id;
    int pdu_size;
};

struct PduWriter {
    std::string type;
    std::string org_name;
    std::string name;
    int write_cycle;
    int channel_id;
    int pdu_size;
    std::string method_type;
};
struct Robot {
    std::string name;
    std::vector<PduReader> pdu_readers;  // 共通化
    std::vector<PduWriter> pdu_writers;  // 共通化
};

struct HakoAssetRunnerType {
    std::string asset_name_str;
    hako_time_t delta_usec;
    hako_time_t current_usec;
    json param;
    std::shared_ptr<hako::IHakoAssetController> hako_asset;
    std::vector<Robot> robots;
};

static HakoAssetRunnerType hako_asset_runner_ctrl;

/***********************
 * simulation control
 ***********************/

static void hako_asset_runner_parse_robots(void)
{
    const json& robots_json = hako_asset_runner_ctrl.param["robots"];
    
    for (const auto& robot_json : robots_json) {
        Robot* robot = new Robot;
        
        // name を取得
        robot->name = robot_json["name"];
        
        // PduReaders を取得
        if (robot_json.find("shm_pdu_readers") != robot_json.end()) {
            const json& pdu_readers_json = robot_json["shm_pdu_readers"];
            for (const auto& reader_json : pdu_readers_json) {
                PduReader* reader = new PduReader{
                    reader_json["type"],
                    reader_json["org_name"],
                    reader_json["name"],
                    reader_json["channel_id"],
                    reader_json["pdu_size"]
                };
                robot->pdu_readers.push_back(*reader);
            }
        } else {
            // nothing to do
        }
        if (robot_json.find("rpc_pdu_readers") != robot_json.end()) {
            const json& pdu_readers_json = robot_json["shm_pdu_readers"];
            for (const auto& reader_json : pdu_readers_json) {
                PduReader* reader = new PduReader{
                    reader_json["type"],
                    reader_json["org_name"],
                    reader_json["name"],
                    reader_json["channel_id"],
                    reader_json["pdu_size"]
                };
                robot->pdu_readers.push_back(*reader);
            }
        } else {
            // nothing to do
        }
        
        // PduWriters を取得
        if (robot_json.find("shm_pdu_writers") != robot_json.end()) {
            const json& pdu_writers_json = robot_json["shm_pdu_writers"];
            for (const auto& writer_json : pdu_writers_json) {
                PduWriter* writer = new PduWriter{
                    writer_json["type"],
                    writer_json["org_name"],
                    writer_json["name"],
                    writer_json["write_cycle"],
                    writer_json["channel_id"],
                    writer_json["pdu_size"],
                    writer_json["method_type"]
                };
                robot->pdu_writers.push_back(*writer);
            }
        } else {
            // nothing to do
        }
        if (robot_json.find("rpc_pdu_writers") != robot_json.end()) {
            const json& pdu_writers_json = robot_json["shm_pdu_writers"];
            for (const auto& writer_json : pdu_writers_json) {
                PduWriter* writer = new PduWriter{
                    writer_json["type"],
                    writer_json["org_name"],
                    writer_json["name"],
                    writer_json["write_cycle"],
                    writer_json["channel_id"],
                    writer_json["pdu_size"],
                    writer_json["method_type"]
                };
                robot->pdu_writers.push_back(*writer);
            }
        } else {
            // nothing to do
        }        
        hako_asset_runner_ctrl.robots.push_back(*robot);
    }
}

bool hako_asset_runner_init(const char* asset_name, const char* config_path, hako_time_t delta_usec)
{
    std::ifstream ifs(config_path);
    
    if (!ifs.is_open()) {
        std::cerr << "Error: Failed to open config file." << std::endl;
        return false;
    }

    hako_asset_runner_ctrl.asset_name_str = asset_name;
    hako_asset_runner_ctrl.delta_usec = delta_usec;
    hako_asset_runner_ctrl.current_usec = 0;
    try {
        hako_asset_runner_ctrl.param = json::parse(ifs);
        hako_asset_runner_parse_robots();
    } catch (const json::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
    hako_asset_runner_ctrl.hako_asset = hako::create_asset_controller();
    if (hako_asset_runner_ctrl.hako_asset == nullptr) {
        std::cerr << "ERROR: Not found hako-master on this PC" << std::endl;
        return false;
    }
    bool ret = hako_asset_runner_ctrl.hako_asset->asset_register_polling(asset_name);
    if (ret == false) {
        std::cerr << "ERROR: Can not register asset" << std::endl;
        return false;
    }
    // PDUのチャネルを作成する
    for (const Robot& robot : hako_asset_runner_ctrl.robots) {
        for (const PduWriter& writer : robot.pdu_writers) {
            std::cout << "Robot: " << robot.name << ", PduWriter: " << writer.name << std::endl;
            std::cout << "channel_id: " << writer.channel_id << " pdu_size: " << writer.pdu_size << std::endl;
            bool err = hako_asset_runner_ctrl.hako_asset->create_pdu_lchannel(
                robot.name,
                writer.channel_id, 
                writer.pdu_size
            );
            if (err == false) {
                std::cerr << "ERROR: Can not create_pdu_channel()" << std::endl;
                return false;
            }            
        }
    }

    return true;
}

void hako_asset_runner_fin(void)
{
    // nothing to do.
}

static bool hako_asset_runner_wait_running(void)
{
    //TODO
    return true;
}
static bool hako_asset_runner_proc(void)
{
    //TODO
    return true;
}
bool hako_asset_runner_step(hako_time_t increment_step)
{
    // WAINT FOR RUNNING STATE
    hako_asset_runner_wait_running();

    // RUNNING PROC
    bool ret = hako_asset_runner_proc();
    if (ret) {
        hako_asset_runner_ctrl.current_usec += ( hako_asset_runner_ctrl.delta_usec * increment_step );
    }

    return true; // 仮の戻り値
}

/***********************
 * pdu io
 ***********************/

bool hako_asset_runner_pdu_read(const char* robo_name, HakoPduChannelIdType lchannel, char* buffer, size_t buffer_len)
{
    bool ret = hako_asset_runner_ctrl.hako_asset->read_pdu(hako_asset_runner_ctrl.asset_name_str, robo_name, lchannel, buffer, buffer_len);
    if (ret == false) {
        return false;
    }
    return true;
}

bool hako_asset_runner_pdu_write(const char* robo_name, HakoPduChannelIdType lchannel, const char* buffer, size_t buffer_len)
{
    bool ret = hako_asset_runner_ctrl.hako_asset->write_pdu(hako_asset_runner_ctrl.asset_name_str, robo_name, lchannel, buffer, buffer_len);
    if (ret == false) {
        return false;
    }
    hako_asset_runner_ctrl.hako_asset->notify_write_pdu_done(hako_asset_runner_ctrl.asset_name_str);
    return true;
}

/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Animation.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Sehoon Kim
Created:    April 15, 2025
*/

#include "Animation.hpp"
#include "Engine/Logger.hpp"
#include "Engine/Path.hpp"
#include <fstream>

CS230::Animation::Animation(const std::filesystem::path& animation_file) : current_command(0)
{
    if (animation_file.extension() != ".anm")
    {
        throw std::runtime_error(animation_file.generic_string() + " is not a .anm file");
    }
    const auto    full_path = assets::locate_asset(animation_file);
    std::ifstream in_file(full_path);
    if (in_file.is_open() == false)
    {
        throw std::runtime_error("Failed to load " + full_path.generic_string());
    }

    std::string command;
    while (in_file.eof() == false)
    {
        in_file >> command;
        if (command == "PlayFrame")
        {
            int    frame_in;
            double target_time_in;
            in_file >> frame_in;
            in_file >> target_time_in;

            commands.push_back(new PlayFrame(frame_in, target_time_in));
        }
        else if (command == "Loop")
        {
            int loop_to_frame_in;
            in_file >> loop_to_frame_in;
            commands.push_back(new Loop(loop_to_frame_in));
        }
        else if (command == "End")
        {
            commands.push_back(new End());
        }
        else
        {
            Engine::GetLogger().LogError(command + " in " + full_path.generic_string());
        }
    }
    Reset();
}

CS230::Animation::Animation() : Animation("./Assets/None.anm")
{
}

CS230::Animation::~Animation()
{
    for (Command* command : commands)
    {
        delete command;
    }
    commands.clear();
}

int CS230::Animation::CurrentFrame()
{
    return current_frame->Frame();
}

bool CS230::Animation::Ended()
{
    return ended;
}

void CS230::Animation::Update(double dt)
{
    current_frame->Update(dt);
    if (current_frame->Ended() == true)
    {
        current_frame->ResetTime();
        current_command++;
        if (commands[current_command]->Type() == CommandType::PlayFrame)
        {
            current_frame = static_cast<PlayFrame*>(commands[current_command]);
        }
        else if (commands[current_command]->Type() == CommandType::Loop)
        {
            Loop* loop_data = static_cast<Loop*>(commands[current_command]);
            current_command = static_cast<std::size_t>(loop_data->LoopIndex());
            if (commands[current_command]->Type() == CommandType::PlayFrame)
            {
                current_frame = static_cast<PlayFrame*>(commands[current_command]);
            }
            else
            {
                Engine::GetLogger().LogError("Loop does not go to PlayFrame");
                Reset();
            }
        }
        else if (commands[current_command]->Type() == CommandType::End)
        {
            ended = true;
        }
    }
}

void CS230::Animation::Reset()
{
    current_command = 0;
    ended           = false;
    current_frame   = static_cast<PlayFrame*>(commands[current_command]);
    current_frame->ResetTime();
}

CS230::Animation::PlayFrame::PlayFrame(int frame_in, double duration) : frame(frame_in), target_time(duration), timer(0)
{
}

void CS230::Animation::PlayFrame::Update(double dt)
{
    timer += dt;
}

bool CS230::Animation::PlayFrame::Ended()
{
    if (timer >= target_time)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int CS230::Animation::PlayFrame::Frame()
{
    return frame;
}

void CS230::Animation::PlayFrame::ResetTime()
{
    timer = 0;
}

CS230::Animation::Loop::Loop(int loop_index_in) : loop_index(loop_index_in)
{
}

int CS230::Animation::Loop::LoopIndex()
{
    return loop_index;
}

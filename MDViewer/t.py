# import json

# result = {}
# with open("C:\\Users\\yuksu\\AppData\\Local\\Packages\\36b4c69c-84cf-4be9-a7fb-56eda70b1b2a_sze2rqgp4k4ww\\LocalState\\userData.json", "r") as f:
#     json_object = json.load(f)
#     for chat in json_object["chatting"]:
#         result[chat] = []
#         for i in json_object["chatting"][chat]["data"]:
#             result[chat].append((i["role"], i["content"]))

# print(result)

# import openai
# import json
# import time
# from datetime import datetime

# openai.api_key = "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"

# def get_current_weather(location, unit="fahrenheit"):
#     """Get the current weather in a given location"""
#     weather_info = {
#         "location": location,
#         "temperature": "72",
#         "unit": unit,
#         "forecast": ["sunny", "windy"],
#     }
#     return json.dumps(weather_info)

# def get_current_time():
#     #today = datetime.today()
#     time_info = {
#         # "year" : today.year,
#         # "month" : today.month,
#         # "day" : today.day,
#         # "hour" : today.hour,
#         # "minute" : today.minute,
#         # "second" : today.second
#         "unix_time" : time.time(),
#         "time_zone" : time.timezone
#     }
#     return json.dumps(time_info)

# def run_program(name):
#     run_info = {
#         "feasibility" : True,
#         "result message" : "run successfully"
#     }
#     return json.dumps(run_info)

# def run_conversation():
#     messages = [{"role": "user", "content": "what time is it now?"}]
#     functions = [
#         {
#             "name": "get_current_weather",
#             "description": "Get the current weather in a given location",
#             "parameters": {
#                 "type": "object",
#                 "properties": {
#                     "location": {
#                         "type": "string",
#                         "description": "The city and state, e.g. San Francisco, CA",
#                     },
#                     "unit": {"type": "string", "enum": ["celsius", "fahrenheit"]},
#                 },
#                 "required": ["location"],
#             },
#         },
#         {
#             "name" : "get_current_time",
#             "description" : "Get current time information",
#             "parameters" : {
#                 "type" : "object",
#                 "properties" : {}
#             }
#         },
#         {
#             "name" : "run_program",
#             "description" : "Executes the program with the given name. If 'result' is true in the result item, the execution was successful.",
#             "parameters" : {
#                 "type" : "object",
#                 "properties" : {
#                     "name" : {
#                         "type" : "string",
#                         "enum" : ["Visual Studio 2022", "Visual Studio Code", "Chrome", "notepad", "2023 plan by ysm.txt"]
#                     }
#                 },
#                 "required": ["name"],
#             }
#         }
#     ]
    
#     response = openai.ChatCompletion.create(
#         model="gpt-3.5-turbo-0613",
#         messages=messages,
#         functions=functions,
#         function_call="auto",
#         #stream=True
#     )
#     # for chunk in response:
#     #     if (chunk["choices"][0]["finish_reason"] == "function_call"):
#     #         break
#     #     print(chunk["choices"][0]["delta"]["function_call"], end="")
    
#     response_message = response["choices"][0]["message"]

#     if response_message.get("function_call"):
#         available_functions = {
#             "get_current_weather": get_current_weather,
#             "get_current_time": get_current_time,
#             "run_program" : run_program
#         }
#         function_name = response_message["function_call"]["name"]
#         fuction_to_call = available_functions[function_name]
#         function_args = json.loads(response_message["function_call"]["arguments"])
#         if function_name == "get_current_weather":
#             function_response = fuction_to_call(
#                 location=function_args.get("location"),
#                 unit=function_args.get("unit"),
#             )
#         elif function_name == "get_current_time":
#             function_response = fuction_to_call()
#         elif function_name == "run_program":
#             function_response = fuction_to_call(name=function_args.get("name"))

#         messages.append(
#             {
#                 "role": "function",
#                 "name": function_name,
#                 "content": function_response,
#             }
#         )  # extend conversation with function response
        
#         second_response = openai.ChatCompletion.create(
#             model="gpt-3.5-turbo-0613",
#             messages=messages,
#         )  # get a new response from GPT where it can see the function response
#         return second_response


# print(run_conversation())


import json

str = "{\"list\":[\"vs_communitymsires\",\"Microsoft ASP.NET Core 3.1.32 Shared Framework (x86)\",\"Microsoft Visual C++ 2022 X86 Additional Runtime - 14.34.31938\",\"Microsoft .NET Runtime - 7.0.5 (x64)\",\"Microsoft Web Deploy 4.0\",\"Microsoft Windows Desktop Targeting Pack - 7.0.5 (x64)\",\"WinRT Intellisense Desktop - Other Languages\",\"Microsoft Visual C++ 2022 X86 Debug Runtime - 14.34.31938\",\"Windows SDK for Windows Store Apps DirectX x86 Remote\",\"BlueStacks App Player\",\"Windows SDK for Windows Store Apps Tools\",\"CMake\",\"IIS Express Application Compatibility Database for x64\",\"Microsoft ASP.NET Core 3.1.23 Shared Framework (x64)\",\"Microsoft ASP.NET Core 5.0.17 Shared Framework (x64)\",\"Windows IoT Extension SDK Contracts\",\"Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729.17\",\"Microsoft Visual C++ 2005 Redistributable\",\"Microsoft .NET Core Runtime - 3.1.23 (x86)\",\"Microsoft .NET Runtime - 5.0.17 (x86)\",\"Microsoft .NET Core AppHost Pack - 3.1.23 (x64_arm64)\",\"Microsoft .NET Native SDK\",\"Universal CRT Headers Libraries and Sources\",\"Windows SDK DirectX x64 Remote\",\"WinRT Intellisense Desktop - en-us\",\"Windows SDK for Windows Store Apps Headers\",\"Microsoft Windows Desktop Runtime - 6.0.16 (x86)\",\"Microsoft Visual Studio Installer\",\"Microsoft .NET Host FX Resolver - 5.0.15 (x86)\",\"Windows Team Extension SDK Contracts\",\"Chrome\",\"X64 Debuggers And Tools\",\"vs_Graphics_Singletonx64\",\"vs_clickoncebootstrappermsires\",\"Microsoft.NET.Sdk.iOS.Manifest-7.0.100 (x64)\",\"WinRT Intellisense IoT - Other Languages\",\"Microsoft Windows Desktop Runtime - 3.1.23 (x86)\",\"Arduino IDE 2.0.3\",\"Python 3.10.2 Core Interpreter (64-bit symbols)\",\"Microsoft .NET Standard Targeting Pack - 2.1.0 (x64)\",\"vcpp_crt.redist.clickonce\",\"WinRT Intellisense PPI - en-us\",\"Microsoft .NET AppHost Pack - 6.0.16 (x64_x86)\",\"Windows Mobile Extension SDK Contracts\",\"Microsoft .NET Core AppHost Pack - 3.1.23 (x64)\",\"TypeScript SDK\",\"Microsoft .NET 6.0 Templates 7.0.203 (x64)\",\"FreeTube 0.18.0\",\"Bandizip\",\"Python 3.10.2 Executables (64-bit symbols)\",\"Windows SDK Desktop Libs x86\",\"Microsoft .NET Framework 4.7.2 SDK(\\\\m\\u0529\",\"Microsoft ASP.NET Core 5.0.15 Shared Framework (x64)\",\"Python 3.10.2 Core Interpreter (64-bit debug)\",\"Application Verifier x64 External Package\",\"GitHub Desktop\",\"Windows SDK Desktop Libs x64\",\"Microsoft.NET.Workload.Mono.Toolchain.net6.Manifest (x64)\",\"Microsoft .NET Core AppHost Pack - 3.1.32 (x64_x86)\",\"Microsoft Visual C++ 2013 x86 Minimum Runtime - 12.0.21005\",\"Microsoft ASP.NET Core 6.0.16 Shared Framework (x86)\",\"Windows SDK Redistributables\",\"\\\\\\ud840\\udc10\\ufffd\\ufffd\",\"vs_BlendMsi\",\"Windows SDK DirectX x86 Remote\",\"Microsoft ASP.NET Core 3.1.10 Targeting Pack (x64)\",\"windows_toolscorepkg\",\"Microsoft ASP.NET Core 7.0.5 Shared Framework (x86)\",\"Windows SDK Desktop Libs arm64\",\"vs_filehandler_amd64\",\"Microsoft Office LTSC Professional Plus 2021 - ko-kr\",\"Microsoft .NET Host - 7.0.5 (x86)\",\"Windows SDK for Windows Store Apps Contracts\",\"Microsoft .NET AppHost Pack - 5.0.15 (x64_x86)\",\"Windows SDK Signing Tools\",\"Microsoft .NET Host FX Resolver - 7.0.5 (x64)\",\"Cheat Engine 7.5\",\"Windows SDK Desktop Headers arm64\",\"Microsoft Visual C++ 2013 x64 Minimum Runtime - 12.0.21005\",\"Windows SDK Desktop Libs arm\",\"Git\",\"Microsoft Windows Desktop Targeting Pack - 6.0.16 (x86)\",\"Universal CRT Tools x64\",\"Microsoft .NET Core 3.1 Templates 5.0.416 (x64)\",\"Windows Phone SDK 8.0 Assemblies for Visual Studio 2019\",\"Microsoft NetStandard SDK\",\"Python 3.10.2 Development Libraries (64-bit)\",\"vs_devenvmsi\",\"Mono for Windows (x64)\",\"Launcher Prerequisites (x64)\",\"Microsoft .NET Runtime - 5.0.15 (x86)\",\"Windows App Certification Kit x64\",\"Arduino IDE\",\"Microsoft .NET Framework 4.6.1 Targeting Pack\",\"Windows Mobile Extension SDK\",\"Microsoft ASP.NET Core 5.0.15 Shared Framework (x86)\",\"Office 16 Click-to-Run Extensibility Component\",\"Windows Team Extension SDK\",\"Java(TM) SE Development Kit 19.0.1 (64-bit)\",\"vs_filehandler_x86\",\"SDK ARM Additions\",\"icecap_collectionresources\",\"Microsoft Visual C++ 2008 Redistributable - x64 9.0.30729.17\",\"WinRT Intellisense UAP - Other Languages\",\"Windows SDK Desktop Headers x86\",\"Visual Studio Community 2022\",\"Microsoft Windows Desktop Targeting Pack - 3.1.0 (x64)\",\"Universal General MIDI DLS Extension SDK\",\"Rockstar Games Social Club\",\"Microsoft .NET SDK 5.0.416 (x64) from Visual Studio\",\"Microsoft Visual C++ 2022 X64 Additional Runtime - 14.34.31938\",\"Microsoft Windows Desktop Targeting Pack - 7.0.5 (x86)\",\"Microsoft Visual Studio 2019 Tools for Unity\",\"MSI Development Tools\",\"Windows SDK EULA\",\"Microsoft .NET Framework 4.7.1 Targeting Pack\",\"Microsoft Visual C++ 2008 Redistributable - x86 9.0.30729\",\"Microsoft.NET.Sdk.Android.Manifest-7.0.100 (x64)\",\"Wondershare PDFelement ( Version 9.5.8 )\",\"Windows SDK for Windows Store Managed Apps Libs\",\"Microsoft .NET 7.0 Templates 7.0.203 (x64)\",\"Microsoft.NET.Sdk.tvOS.Manifest-7.0.100 (x64)\",\"Microsoft Windows Desktop Runtime - 5.0.15 (x86)\",\"Microsoft Visual C++ 2013 Redistributable (x64) - 12.0.30501\",\"vs_FileTracker_Singleton\",\"Microsoft .NET Framework 4.8 Developer Pack\",\"Visual C++ Library CRT Appx Resource Package\",\"SQL Server 2019 CTP2.2\\u0260Microsoft System CLR Types\",\"Kits Configuration Installer\",\"Microsoft .NET Toolset 5.0.406 (x64)\",\"Microsoft Windows Desktop Runtime - 3.1.23 (x64)\",\"Windows SDK Desktop Headers x64\",\"Python 3.10.2 Executables (64-bit debug)\",\"Microsoft .NET Core 5.0 Templates 5.0.416 (x64)\",\"Office 16 Click-to-Run Localization Component\",\"Logitech G HUB\",\"Microsoft 365 - ko-kr\",\"Windows SDK\",\"icecap_collectionresourcesx64\",\"vs_SQLClickOnceBootstrappermsi\",\"vs_communitysharedmsi\",\"Microsoft .NET AppHost Pack - 6.0.16 (x64_arm)\",\"Microsoft Visual C++ 2012 x64 Minimum Runtime - 11.0.61030\",\"Microsoft Visual C++ 2015-2022 Redistributable (x64) - 14.34.31938\",\"Microsoft Windows Desktop Runtime - 7.0.5 (x64)\",\"Windows Subsystem for Linux Update\",\"Microsoft Visual C++ 2013 Redistributable (x86) - 12.0.30501\",\"vs_minshellinteropmsi\",\"Microsoft ASP.NET Core 5.0.0 Targeting Pack (x64)\",\"Microsoft .NET Core Targeting Pack - 3.1.0 (x64)\",\"Windows SDK Desktop Headers arm\",\"Microsoft.NET.Sdk.macOS.Manifest-7.0.100 (x64)\",\"IntelliTraceProfilerProxy\",\"Microsoft.NET.Workload.Emscripten.net6.Manifest (x64)\",\"Microsoft .NET Host FX Resolver - 5.0.17 (x86)\",\"Python 3.10.2 Tcl/Tk Support (64-bit symbols)\",\"Microsoft .NET Framework 4.7.2 Targeting Pack\",\"WinRT Intellisense IoT - en-us\",\"Microsoft ASP.NET Core 7.0.5 Shared Framework (x64)\",\"VS Script Debugging Common\",\"Entity Framework 6.2.0 Tools  for Visual Studio 2022\",\"Microsoft ASP.NET Core 5.0.17 Shared Framework (x86)\",\"Microsoft ASP.NET Core 3.1.23 Shared Framework (x86)\",\"Windows Desktop Extension SDK Contracts\",\"Windows Software Development Kit - Windows 10.0.22000.832\",\"Windows SDK ARM Desktop Tools\",\"Microsoft .NET Framework 4.5.2 Multi-Targeting Pack\",\"Microsoft Visual C++ 2022 X64 Minimum Runtime - 14.34.31938\",\"Python 3.10.2 Executables (64-bit)\",\"Python 3.10.2 Standard Library (64-bit symbols)\",\"WinRT Intellisense PPI - Other Languages\",\"Entity Framework 6.2.0 Tools  for Visual Studio 2019\",\"Microsoft Edge WebView2 \\ufffd\\ufffd\",\"tt$\\ufffd\",\"vs_communityx64msi\",\"Universal CRT Extension SDK\",\"Python 3.10.2 Test Suite (64-bit debug)\",\"Visual C++ Library CRT Desktop Appx Package\",\"Microsoft .NET Core Host - 3.1.23 (x64)\",\"BlueStacks X\",\"Microsoft .NET AppHost Pack - 5.0.17 (x64_x86)\",\"SQL Server 2019\\u0260Microsoft System CLR Types\",\"Microsoft ASP.NET Core 7.0.5 Targeting Pack (x86)\",\"nProtect Online Security V1.0(PFS)\",\"Android Studio\",\"Python 3.10.2 Core Interpreter (64-bit)\",\"Windows SDK for Windows Store Apps Libs\",\"Steam\",\"vs_clickoncesigntoolmsi\",\"Microsoft Edge\",\"Node.js\",\"Teams Machine-Wide Installer\",\"vs_networkemulationmsi_x64\",\"Python 3.10.2 Add to Path (64-bit)\",\"Windows SDK Facade Windows WinMD Versioned\",\"Microsoft Visual C++ 2022 X86 Minimum Runtime - 14.34.31938\",\"libsndfile version 1.0.28\",\"Microsoft .NET Targeting Pack - 7.0.5 (x64)\",\"Microsoft Windows Desktop Targeting Pack - 6.0.16 (x64)\",\" \\u0218 HTTPS\",\"Microsoft OneDrive\",\"Microsoft .NET Framework 4.7.2 SDK\",\"Microsoft .NET Runtime - 5.0.17 (x64)\",\"OBS Studio\",\"WinRT Intellisense UAP - en-us\",\"Python Launcher\",\"Microsoft .NET SDK 7.0.203 (x64) from Visual Studio\",\"Windows SDK Modern Versioned Developer Tools\",\"VS Immersive Activate Helper\",\"Universal CRT Tools x86\",\"Microsoft UniversalWindowsPlatform SDK\",\"X86 Debuggers And Tools\",\"VS JIT Debugger\",\"PyCharm Community Edition 2021.2.3\",\"Microsoft .NET Framework 4.6 Targeting Pack\",\"Panda3D Game Engine\",\"Application Verifier x64 External Package (OnecoreUAP)\",\"Microsoft ASP.NET Core 7.0.5 Targeting Pack (x64)\",\"\\\\\\ud853\\udf24 2020\",\"Visual Studio\\u0260Microsoft .NET Framework Cumulative Intellisense )(\\\\m\\u0529\",\"Microsoft .NET Framework 4.8 Targeting Pack\",\"Windows SDK Desktop Tools arm64\",\"vs_communitymsi\",\"Visual C++ Library CRT Appx Package\",\"Universal CRT Redistributable\",\"Microsoft Visual Studio Setup Configuration\",\"vs_minshellinteropsharedmsi\",\"Microsoft .NET AppHost Pack - 5.0.15 (x64_arm)\",\"Microsoft DirectX SDK (June 2010)\",\"Windows SDK for Windows Store Apps Metadata\",\"Lenovo Vantage Service\",\"Microsoft.NET.Workload.Emscripten.net7.Manifest (x64)\",\"vs_minshellinteropx64msi\",\"Microsoft .NET AppHost Pack - 7.0.5 (x64_x86)\",\"Microsoft .NET Host FX Resolver - 5.0.17 (x64)\",\"Windows Desktop Extension SDK\",\"Discord\",\"Microsoft Visual C++ 2015-2022 Redistributable (x86) - 14.34.31938\",\"Microsoft .NET Framework 4.5.1 Multi-Targeting Pack\",\"Microsoft Windows Desktop Runtime - 6.0.16 (x64)\",\"Microsoft .NET Toolset 5.0.416 (x64)\",\"Office 16 Click-to-Run Licensing Component\",\"Microsoft .NET Framework 4.8 SDK(\\\\m\\u0529\",\"Microsoft Visual C++ 2012 x86 Additional Runtime - 11.0.61030\",\"Miniconda3 py38_4.9.2 (Python 3.8.5 64-bit)\",\"vs_codecoveragemsi\",\"Microsoft .NET Core SDK 3.1.417 (x64)\",\"Microsoft .NET Targeting Pack - 7.0.5 (x86)\",\"Microsoft .NET Targeting Pack - 6.0.16 (x86)\",\"Microsoft .NET Framework 4.7.1 8\\u001c \\u0314\\tX \\ufffd\\u0005 )(\\\\m\\u0529\",\"Microsoft SQL Server 2019 LocalDB\",\"Microsoft .NET Framework 4.8 SDK\",\"Microsoft Visual C++ 2010  x86 Redistributable - 10.0.30319\",\"IIS Express Application Compatibility Database for x86\",\"vs_devenx64vmsi\",\"Steamworks Common Redistributables\",\"vs_tipsmsi\",\"Microsoft Visual C++ 2012 x64 Additional Runtime - 11.0.61030\",\"Microsoft Visual C++ 2022 X64 Debug Runtime - 14.34.31938\",\"ClickOnce Bootstrapper Package for Microsoft .NET Framework 4.8 on Visual Studio 2017\",\"Microsoft .NET AppHost Pack - 7.0.5 (x64_arm64)\",\"Windows SDK Desktop Tools x86\",\"ClickOnce Bootstrapper Package for Microsoft .NET Framework\",\"Microsoft .NET Framework 4.8 Targeting Pack (ENU)\",\"Microsoft .NET Targeting Pack - 6.0.16 (x64)\",\"vs_clickoncebootstrappermsi\",\"Microsoft Windows Desktop Targeting Pack - 5.0.0 (x64)\",\"Microsoft .NET Toolset 7.0.203 (x64)\",\"Windows SDK Modern Non-Versioned Developer Tools\",\"Microsoft ASP.NET Core 6.0.16 Shared Framework (x64)\",\"vs_minshellmsires\",\"Microsoft 365 Apps for enterprise - en-us\",\"Microsoft .NET Core Runtime - 3.1.23 (x64)\",\"icecap_collection_neutral\",\"iniLINE CrossEX Service\",\"Application Verifier x64 External Package (DesktopEditions)\",\"Microsoft Visual C++ 2012 Redistributable (x86) - 11.0.61030\",\"Windows Simulator\",\"Python 3.10.2 Tcl/Tk Support (64-bit)\",\"\\u0530\\u0004|t\\u0229 Microsoft 365 q - ko-kr\",\"WinAppDeploy\",\"Microsoft Update Health Tools\",\"Microsoft Windows Desktop Runtime - 3.1.32 (x64)\",\"Microsoft Visual C++ 2013 x86 Additional Runtime - 12.0.21005\",\"Python 3.10.2 Utility Scripts (64-bit)\",\"Microsoft .NET Framework 4.7.2 \\ufffd\\u0005 )(\\\\m\\u0529\",\"Microsoft Visual C++ 2010  x64 Redistributable - 10.0.30319\",\"Microsoft .NET AppHost Pack - 6.0.16 (x64_arm64)\",\"Microsoft ASP.NET Core 3.1.32 Shared Framework (x64)\",\"Microsoft .NET AppHost Pack - 7.0.5 (x64_arm)\",\"Python 3.10.2 (64-bit)\",\"Windows IoT Extension SDK\",\"Microsoft .NET Runtime - 6.0.16 (x64)\",\"vs_Graphics_Singletonx86\",\"DB Browser for SQLite\",\"Microsoft .NET Core AppHost Pack - 3.1.32 (x64)\",\"Microsoft .NET AppHost Pack - 5.0.15 (x64_arm64)\",\"Windows SDK Desktop Tools x64\",\"Microsoft .NET Core AppHost Pack - 3.1.23 (x64_x86)\",\"vs_vswebprotocolselectormsi\",\"Windows SDK AddOn\",\"Microsoft .NET Host - 7.0.5 (x64)\",\"Python 3.10.2 Standard Library (64-bit)\",\"Microsoft .NET Runtime - 7.0.5 (x86)\",\"Python 3.10.2 Documentation (64-bit)\",\"Tools for .Net 3.5 - KOR Lang Pack\",\"vs_CoreEditorFonts\",\"Python 3.10.2 Tcl/Tk Support (64-bit debug)\",\"IIS 10.0 Express\",\"Microsoft Windows Desktop Runtime - 7.0.5 (x86)\",\"Microsoft ASP.NET Core 6.0.16 Targeting Pack (x86)\",\"Microsoft .NET Host - 5.0.17 (x64)\",\"Samsung Portable SSD Software 1.0\",\"Microsoft .NET Runtime - 5.0.15 (x64)\",\"Microsoft TestPlatform SDK Local Feed\",\" \",\"Microsoft.NET.Workload.Mono.Toolchain.net7.Manifest (x64)\",\"Microsoft Windows Desktop Runtime - 3.1.32 (x86)\",\"Microsoft.NET.Sdk.MacCatalyst.Manifest-7.0.100 (x64)\",\"Microsoft .NET Core Toolset 3.1.417 (x64)\",\"Microsoft .NET Core 3.1 Templates 5.0.406 (x64)\",\"Visual C++ Library CRT ARM64 Appx Package\",\"Microsoft .NET Runtime - 6.0.16 (x86)\",\"Microsoft Office LTSC Professional Plus 2021 - en-us\",\"Microsoft Bing Service\",\"Python 3.10.2 Test Suite (64-bit)\",\"Microsoft .NET AppHost Pack - 5.0.15 (x64)\",\"Windows 11 $X \\u0130\\ufffd\",\"Python 3.10.2 Test Suite (64-bit symbols)\",\"UE4 Prerequisites (x64)\",\"Microsoft Windows Desktop Runtime - 5.0.17 (x64)\",\"Microsoft Edge Update\",\"SDK ARM Redistributables\",\"Windows Software Development Kit - Windows 10.0.19041.685\",\"vs_minshellmsi\",\"Microsoft Visual C++ 2012 Redistributable (x64) - 11.0.61030\",\"Microsoft Visual C++ 2013 x64 Additional Runtime - 12.0.21005\",\"Microsoft .NET Core 3.1 Templates 3.1.417 (x64)\",\"IEPageSetupX\",\"Windows App Certification Kit SupportedApiList x86\",\"Apple Mobile Device Support\",\"Microsoft .NET Targeting Pack - 5.0.0 (x64)\",\"Epic Online Services\",\"Tools for .Net 3.5\",\"Microsoft SQL Server 2016 LocalDB\",\"Microsoft.NET.Sdk.Maui.Manifest-7.0.100 (x64)\",\"Microsoft .NET CoreRuntime For CoreCon\",\"Microsoft ASP.NET Core 6.0.16 Targeting Pack (x64)\",\"Microsoft .NET AppHost Pack - 5.0.17 (x64_arm64)\",\"Microsoft .NET AppHost Pack - 6.0.16 (x64)\",\"Windows Software Development Kit - Windows 10.0.22621.755\",\"Microsoft .NET Core AppHost Pack - 3.1.32 (x64_arm64)\",\"icecap_collection_x64\",\"Microsoft Visual C++ 2012 x86 Minimum Runtime - 11.0.61030\",\"vs_vswebprotocolselectormsires\",\"WinRT Intellisense Mobile - en-us\",\"Windows SDK for Windows Store Apps\",\"Visual Studio Community 2019\",\"Microsoft Visual Studio Setup WMI Provider\",\"Microsoft .NET Core Runtime - 3.1.32 (x86)\",\"Microsoft Windows Desktop Runtime - 5.0.15 (x64)\",\"DiagnosticsHub_CollectionService\",\"Windows App Certification Kit x64 (OnecoreUAP)\",\"Grand Theft Auto V\",\"Microsoft .NET Host FX Resolver - 7.0.5 (x86)\",\"Microsoft Windows Desktop Runtime - 5.0.17 (x86)\",\"Python 3.10.2 Development Libraries (64-bit debug)\",\"Microsoft .NET Framework 4 Multi-Targeting Pack\",\"Microsoft .NET CoreRuntime SDK\",\"vs_devenvsharedmsi\",\"Microsoft .NET AppHost Pack - 5.0.17 (x64_arm)\",\"Microsoft .NET Core AppHost Pack - 3.1.23 (x64_arm)\",\"Python 3.10.2 Standard Library (64-bit debug)\",\"Microsoft .NET Core AppHost Pack - 3.1.32 (x64_arm)\",\"Microsoft .NET Core 5.0 Templates 5.0.406 (x64)\",\"Windows App Certification Kit Native Components\",\"Microsoft Visual Studio Code (User)\",\"vs_minshellsharedmsi\",\"Rockstar Games Launcher\",\"Microsoft .NET AppHost Pack - 7.0.5 (x64)\",\"IPinside LWS Agent\",\"Microsoft .NET AppHost Pack - 5.0.17 (x64)\",\"Microsoft .NET Host FX Resolver - 5.0.15 (x64)\",\"vs_minshellx64msi\",\"Microsoft .NET Core Host FX Resolver - 3.1.23 (x64)\",\"Microsoft .NET Framework 4.8 \\ufffd\\u0005 )(\\\\m\\u0529\",\"Windows Simulator - ENU\",\"Microsoft .NET Core Runtime - 3.1.32 (x64)\",\"Python 3.10.2 pip Bootstrap (64-bit)\",\"Vulkan SDK 1.3.216.0\",\"Microsoft .NET Framework 4.5 Multi-Targeting Pack\"]}\n"

data = json.loads(str)
with open("re.json", "w") as file:
    json.dump(data, file)
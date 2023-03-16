#!/usr/bin/env python3

import datetime
import subprocess
import sys
import re
import serial
import numpy as np
from config import Settings
import os.path

def toLog(name, value, k=None):
  if value > 20000:
    value = f"{round(value/1000)}k"
  else:
    value = f"{value}"
  return f"{name}: {value}\n"

def toMacro(name, value, k=None):
  if value > 20000:
    value = f"{round(value/1000):,}k"
  else:
    value = f"{value:,}"
  value = value.replace(",", "\\,")
  return f"\\DefineVar{{{name}}}{{{value}}}\n"

def run_bench(scheme_path, scheme_name, scheme_type):
    subprocess.check_call(f"make clean", shell=True)
    subprocess.check_call(f"make CRYPTO_PATH={scheme_path} stack", shell=True)
    try:
        subprocess.check_call(f"make run", shell=True)
    except:
        print("flash failed --> retry")
        return run_bench(scheme_path, scheme_name, scheme_type)

    # get serial output and wait for '#'
    with serial.Serial(Settings.SERIAL_DEVICE, 115200, timeout=1000) as dev:
        logs = []
        iteration = 0
        log = b""
        while iteration < 1:
            device_output = dev.read()
            if device_output == b'':
                print("timeout --> retry")
                return run_bench(scheme_path, scheme_name, scheme_type)
            sys.stdout.buffer.write(device_output)
            sys.stdout.flush()
            log += device_output
            if device_output == b'#':
                logs.append(log)
                log = b""
                iteration += 1
    return logs


def parseLogSpeed(log, ignoreErrors):
    log = log.decode(errors="ignore")
    if "error" in log.lower() and not ignoreErrors:
        raise Exception("error in scheme. this is very bad.")
    lines = str(log).splitlines()
    def get(lines, key):
        if key in lines:
            return int(lines[1+lines.index(key)])
        else:
            return None

    def cleanNullTerms(d):
        return {
            k:v
            for k, v in d.items()
            if v is not None
        }

    return cleanNullTerms({
        f"keygen":  get(lines, "keypair stack:"),
        f"encaps":  get(lines, "encaps stack:"),
        f"decaps":  get(lines, "decaps stack:")
    })

def average(results):
    avgs = dict()
    for key in results[0].keys():
        avgs[key] = int(np.array([results[i][key] for i in range(len(results))]).mean())
    return avgs


def bench(scheme_path, scheme_name, scheme_type, outfile, ignoreErrors=False):
    logs    = run_bench(scheme_path, scheme_name, scheme_type)
    results = []
    for log in logs:
        try:
            result = parseLogSpeed(log, ignoreErrors)
        except:
            breakpoint()
            print("parsing log failed -> retry")
            return bench(scheme_path, scheme_name, scheme_type, outfile)
        results.append(result)
    avgResults = average(results)
    print(f"%RISC-V results for {scheme_name} (type={scheme_type})", file=outfile)
    scheme_nameStripped = scheme_name.replace("-", "") 
    for key, value in avgResults.items():
        macro = toMacro(f"{scheme_nameStripped}{key}", value)
        print(macro.strip())
        print(macro, end='', file=outfile)
    print('', file=outfile, flush=True)


with open(f"stack_benchmarks.txt", "a") as outfile:

    now = datetime.datetime.now(datetime.timezone.utc)
    # iterations = 100 # defines the number of measurements to perform
    print(f"% Benchmarking measurements written on {now}\n", file=outfile)

    subprocess.check_call(f"make clean", shell=True)

    # uncomment the scheme variants that should be build and evaluated
    for scheme_path in [
        "crypto_kem/kyber512/fstack",
        "crypto_kem/kyber512/fspeed",
        "crypto_kem/kyber768/fstack",
        "crypto_kem/kyber768/fspeed",
        "crypto_kem/kyber1024/fstack",
        "crypto_kem/kyber1024/fspeed"
    ]:
        scheme_name = scheme_path.replace("/", "_")
        scheme_type = re.search('crypto_(.*?)_', scheme_name).group(1)
        bench(scheme_path, scheme_name, scheme_type, outfile)





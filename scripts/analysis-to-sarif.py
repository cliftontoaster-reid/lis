#!/usr/bin/env python3
"""Convert clang-tidy text output and cppcheck XML to one SARIF document."""

import argparse
import json
import os
import re
import xml.etree.ElementTree as ET


CLANG_TIDY_DIAGNOSTIC = re.compile(
    r"^(.*):(\d+):(\d+): (warning|error|note): (.*?) \[([^\]]+)\]$"
)


def location(path, line, column):
    if os.path.isabs(path):
        path = os.path.relpath(path, os.getcwd())
    return {
        "physicalLocation": {
            "artifactLocation": {"uri": path.replace(os.sep, "/")},
            "region": {"startLine": int(line), "startColumn": max(1, int(column))},
        }
    }


def result(rule_id, message, level, path, line, column):
    return {
        "ruleId": rule_id,
        "level": level,
        "message": {"text": message},
        "locations": [location(path, line, column)],
    }


def parse_clang_tidy(path):
    findings = []
    with open(path, encoding="utf-8", errors="replace") as stream:
        for raw_line in stream:
            match = CLANG_TIDY_DIAGNOSTIC.match(raw_line.rstrip())
            if not match:
                continue
            file_name, line, column, severity, message, rule_id = match.groups()
            findings.append(
                result(
                    rule_id,
                    message,
                    "error" if severity == "error" else "warning",
                    file_name,
                    int(line),
                    int(column),
                )
            )
    return findings


def parse_cppcheck(path):
    findings = []
    root = ET.parse(path).getroot()
    for error in root.findall("./errors/error"):
        locations = error.findall("./location")
        if not locations:
            continue
        first = locations[0]
        severity = error.get("severity", "warning")
        level = "error" if severity in {"error", "fatal"} else "warning"
        findings.append(
            result(
                error.get("id", "cppcheck"),
                error.get("verbose", error.get("msg", "cppcheck finding")),
                level,
                first.get("file", ""),
                int(first.get("line", "1")),
                int(first.get("column", "1")),
            )
        )
    return findings


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--clang-tidy", required=True)
    parser.add_argument("--cppcheck", required=True)
    parser.add_argument("--output", required=True)
    args = parser.parse_args()

    document = {
        "$schema": "https://json.schemastore.org/sarif-2.1.0.json",
        "version": "2.1.0",
        "runs": [
            {
                "tool": {
                    "driver": {
                        "name": "clang-tidy",
                        "informationUri": "https://clang.llvm.org/extra/clang-tidy/",
                    }
                },
                "results": parse_clang_tidy(args.clang_tidy),
            },
            {
                "tool": {
                    "driver": {
                        "name": "cppcheck",
                        "informationUri": "https://cppcheck.sourceforge.io/",
                    }
                },
                "results": parse_cppcheck(args.cppcheck),
            },
        ],
    }
    with open(args.output, "w", encoding="utf-8") as stream:
        json.dump(document, stream, indent=2)
        stream.write("\n")


if __name__ == "__main__":
    main()

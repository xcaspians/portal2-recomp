#!/usr/bin/env python3
"""Turns the SystemCapabilities that XcodeGen wrote as text back into a dictionary.

project-icloud.yml marks iCloud on the Portal2 target (TargetAttributes >
SystemCapabilities, what Xcode's Signing & Capabilities tab writes). XcodeGen
2.46 writes any nested value of a target's `attributes` as the Swift text of
the dictionary:

    SystemCapabilities = "[\\"com.apple.iCloud\\": [\\"enabled\\": 1]]";

whatever the spelling in YAML or JSON. Xcode then can't read the capability:
its Signing & Capabilities tab neither shows nor adds iCloud. Run after
generation (the spec's options.postGenCommand), this rewrites it as

    SystemCapabilities = {
        com.apple.iCloud = {
            enabled = 1;
        };
    };

and leaves everything else in the file as it was.
    python3 capabilities.py P2Signing.xcodeproj/project.pbxproj
"""
import re
import sys

LINE = re.compile(r'^(?P<indent>[ \t]*)SystemCapabilities = "(?P<text>(?:[^"\\]|\\.)*)";[ \t]*$', re.M)


class _Reader:
    """A Swift dictionary literal as XcodeGen prints it: ["key": value, ...],
    values being dictionaries, integers, true/false or strings."""

    def __init__(self, text):
        self.text, self.at = text, 0

    def fail(self, what):
        raise ValueError(f'{what} at {self.at} in {self.text!r}')

    def skip(self):
        while self.at < len(self.text) and self.text[self.at] in ' \t\n':
            self.at += 1

    def expect(self, char):
        self.skip()
        if self.text[self.at:self.at + 1] != char:
            self.fail(f'expected {char!r}')
        self.at += 1

    def string(self):
        self.expect('"')
        out = []
        while self.at < len(self.text) and self.text[self.at] != '"':
            if self.text[self.at] == '\\':
                self.at += 1
            out.append(self.text[self.at])
            self.at += 1
        self.expect('"')
        return ''.join(out)

    def value(self):
        self.skip()
        head = self.text[self.at:self.at + 1]
        if head == '[':
            return self.dictionary()
        if head == '"':
            return self.string()
        word = re.compile(r'-?[0-9]+|true|false').match(self.text, self.at)
        if not word:
            self.fail('unknown value')
        self.at = word.end()
        return {'true': 1, 'false': 0}[word.group()] if word.group() in ('true', 'false') else int(word.group())

    def dictionary(self):
        self.expect('[')
        out = {}
        self.skip()
        if self.text.startswith(':]', self.at):   # Swift's empty dictionary
            self.at += 2
            return out
        while True:
            key = self.string()
            self.expect(':')
            out[key] = self.value()
            self.skip()
            if self.text[self.at:self.at + 1] == ',':
                self.at += 1
                continue
            self.expect(']')
            return out


def parse(text):
    """The dictionary of XcodeGen's text (its escaped quotes already undone)."""
    reader = _Reader(text)
    value = reader.dictionary()
    reader.skip()
    if reader.at != len(text):
        reader.fail('trailing text')
    return value


def _plist_key(key):
    return key if re.fullmatch(r'[A-Za-z0-9_./]+', key) else '"' + key.replace('"', '\\"') + '"'


def render(value, indent, step='\t'):
    """OpenStep plist text of a dictionary, as Xcode writes the project file."""
    if isinstance(value, dict):
        lines = ['{']
        for key, item in value.items():
            lines.append(f'{indent}{step}{_plist_key(key)} = {render(item, indent + step, step)};')
        lines.append(f'{indent}}}')
        return '\n'.join(lines)
    return _plist_key(str(value))


def fix(pbxproj):
    """The project file's text with every stringified SystemCapabilities made a dictionary."""
    def repl(m):
        text = m.group('text').replace('\\"', '"').replace('\\\\', '\\')
        return f'{m.group("indent")}SystemCapabilities = {render(parse(text), m.group("indent"))};'
    return LINE.sub(repl, pbxproj)


def main(argv):
    if len(argv) != 2:
        sys.exit(f'usage: {argv[0]} <project.pbxproj>')
    with open(argv[1], encoding='utf-8') as f:
        before = f.read()
    after = fix(before)
    if after != before:
        with open(argv[1], 'w', encoding='utf-8') as f:
            f.write(after)


if __name__ == '__main__':
    main(sys.argv)

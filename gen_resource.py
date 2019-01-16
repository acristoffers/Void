#/usr/bin/env python3

import os

template = """
<RCC>
    <qresource prefix="/">
        <file alias="icon.png">res/icon.png</file>
        <file alias="icon.svg">res/icon.svg</file>
        $FILES
    </qresource>
</RCC>
""".strip()

fs = [os.path.join(p, f) for p, _, sf in os.walk('html') for f in sf]
fs = [f'<file>{f}</file>' for f in fs]

template = template.replace('$FILES', '\n        '.join(fs))

with open('resources.qrc', 'w') as file:
    file.write(template)

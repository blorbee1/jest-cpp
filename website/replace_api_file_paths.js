const fs = require("fs");
const path = require("path");

const xmlDir = "doxygen/xml";
const repoUrl = "https://github.com/blorbee1/jest-cpp/blob/master/";

fs.readdirSync(xmlDir).forEach(file => {
    if (file.endsWith(".xml")) {
        const p = path.join(xmlDir, file);
        let content = fs.readFileSync(p, 'utf8');
        content = content.replace(/location file="([^"]+)" line="(\d+)"/g, (m, fPath, lineNum) => {
            let cleanPath = path.isAbsolute(fPath)
                ? path.relative(process.cwd(), fPath)
                : fPath;

            cleanPath = cleanPath.replace(/\\/g, '/').replace(/^(\.\.\/|\.\/)+/, '');
            return `location file="${repoUrl}${cleanPath}#L${lineNum}?" line="${lineNum}"`;
        });
        fs.writeFileSync(p, content);
    }
})
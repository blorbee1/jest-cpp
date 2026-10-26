// @ts-check
const ORG = "blorbee1";
const REPO = "jest-cpp";

/** @type {import('@docusaurus/types').Config} */
const config = {
    title: "jest-cpp",
    tagline: "Jest style unit testing for C++20, in a single header",

    url: `https://${ORG}.github.io`,
    baseUrl: `/${REPO}/`,
    organizationName: ORG,
    projectName: REPO,
    trailingSlash: false,

    onBrokenLinks: "throw",
    markdown: {
        format: "detect",
        hooks: {onBrokenMarkdownLinks: "throw"},
    },

    presets: [
        [
            "classic",
            /** @type {import("@docusaurus/preset-classic").Options} */
            ({
                docs: {
                    path: "../docs",
                    sidebarPath: "./sidebars.js",
                    editUrl: ({docPath}) => `https://github.com/${ORG}/${REPO}/edit/master/docs/${docPath}`
                },
                blog: false,
                theme: {customCss: "./src/css/custom.css"}
            })
        ]
    ],

    themes: [
        [
            '@easyops-cn/docusaurus-search-local',
            /** @type {import("@easyops-cn/docusaurus-search-local").PluginOptions} */
            ({
                hashed: true,
                indexBlog: false,
                docsRouteBasePath: "/docs",
                docsDir: "../docs",
                highlightSearchTermsOnTargetPage: true
            })
        ]
    ],

    themeConfig:
        /** @type {import("@docusaurus/preset-classic").ThemeConfig} */
        ({
            colorMode: {respectPrefersColorScheme: true},
            navbar: {
                title: "jest-cpp",
                items: [
                    {type: "docSidebar", sidebarId: "docs", position: "right", label: "Docs"},
                    {to: "docs/api", position: "right", label: "API"},
                    {"href": `https://github.com/${ORG}/${REPO}`, label: "GitHub", position: "right"}
                ]
            },
            footer: {
                style: "dark",
                links: [
                    {
                        title: "Docs",
                        items: [
                            {label: "Getting started", to: "/docs/getting-started"},
                            {label: "API reference", to: "/docs/api"},
                            {label: "Differences from JestJS", to: "/docs/differences-from-jest"}
                        ]
                    },
                    {
                        title: "Project",
                        items: [
                            {label: "GitHub", href: `https://github.com/${ORG}/${REPO}`},
                            {label: "Issues", href: `https://github.com/${ORG}/${REPO}/issues`},
                            {label: "Changelog", href: `https://github.com/${ORG}/${REPO}/blob/master/CHANGELOG.md`}
                        ]
                    }
                ],
                copyright: `Copyright © ${new Date().getFullYear()} jest-cpp contributors. MIT licensed. Built with Docusaurus.`
            },
            prism: {
                additionalLanguages: ["cpp", "cmake", "bash"]
            }
        })
};

module.exports = config;
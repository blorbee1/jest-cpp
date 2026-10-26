# Documentation site

Build with [Docusaurus](https://docusaurus.io/) and deployed to GitHub Pages via `.github/workflows/docs.yml`

```sh
cd website
npm ci          # first time only (if node_modules is missing)
npm start       # live preview at http://localhost:3000/jest-cpp/
npm run build   # production build into website/build (fails on broken links)
npm run serve   # serve the production build locally
```

- Add a page: create `docs/my-page.md` (the first `# Heading` is its title), then add `'my-page'` to `sidebars.js`.
- Plain `.md` files are CommonMark, so `{`, `}` and `<regex>` in prose are safe. Use `.mdx` only when you need React components.
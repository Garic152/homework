import Base: show, *


struct Pfad
  source::Real
  target::Union{Real,Pfad}
end


function pfad(source::Real, target::Real)::Pfad
  return Pfad(source, target)
end


function pfad(source::Real, target::Pfad)::Pfad
  return Pfad(source, target)
end


function pfad(source::Real)::Pfad
  return Pfad(source, source)
end


⇒(source::Real, target::Real) = pfad(source, target)
⇒(source::Real, target::Pfad) = pfad(source, target)


function show(io::IO, p::Pfad)
  if p.target isa Pfad
    print(io, "$(p.source) ⇒ ")
    show(io, p.target)
  else
    print(io, "$(p.source) ⇒ $(p.target)")
  end
end


function *(f::Pfad, g::Pfad)::Pfad
  if f.target isa Pfad
    new_target = f.target * g
  else
    @assert f.target == g.source
    new_target = g
  end
  return Pfad(f.source, new_target)
end


a = pfad(2, 3)
b = pfad(4, a)
c = pfad(1)

println(a, b, c)

